use crate::engine::config::Configuration;
use crate::engine::rng::EngineRng;
use crate::engine::util::*;
use std::collections::HashSet;

//=============================================
// Pipe Implementation
//=============================================

/// Represents a pipe to be rendered. This is a ordered
/// list of nodes that the pipe occupies. Other properties
/// related to the pipe (such as color) are not stored here.
///
pub struct Pipe {
    alive: bool,
    nodes: Vec<Coordinate>,
    current_dir: Direction,
    space_bounds: Coordinate,
}

impl Pipe {
    pub fn new(
        occupied_nodes: &mut HashSet<Coordinate>,
        bounds: Coordinate,
        rng: &mut impl EngineRng,
    ) -> Pipe {
        let mut new_pipe: Pipe = Pipe {
            alive: true,
            nodes: Vec::new(),
            current_dir: choose_random_direction(rng),
            space_bounds: bounds,
        };

        let start = find_random_start(occupied_nodes, bounds, rng);
        new_pipe.nodes.push(start);
        occupied_nodes.insert(start);

        new_pipe
    }

    pub fn is_alive(&self) -> bool {
        self.alive
    }

    pub fn kill(&mut self) {
        self.alive = false;
    }

    /// Returns the current head of the pipe, which is the first element in the nodes vector.
    /// We always assume that when adding a new node to the pipe, it is added to the front of the
    /// vector.
    pub fn get_current_head(&self) -> Coordinate {
        self.nodes[0]
    }

    pub fn get_current_dir(&self) -> Direction {
        self.current_dir
    }

    pub fn len(&self) -> i32 {
        self.nodes.len().try_into().unwrap()
    }

    pub fn update(
        &mut self,
        occupied_nodes: &mut HashSet<Coordinate>,
        rng: &mut impl EngineRng,
        turn_chance: Option<f32>,
    ) {
        if !self.alive {
            return;
        }

        let turn_float: f64 = match turn_chance {
            Some(chance) => chance.into(),
            None => 1.0 / 2.0,
        };

        let directions_to_try = self.get_directions(rng, turn_float);

        match self.get_next_pos(directions_to_try, occupied_nodes) {
            Some(pos) => {
                occupied_nodes.insert(pos);
                self.nodes.insert(0, pos);
            }
            None => {
                self.kill();
                return;
            }
        }
    }

    ///Portion of update logic related to generating the random
    /// list of directions to check for the next node.
    fn get_directions(&mut self, rng: &mut impl EngineRng, turn_float: f64) -> Vec<Direction> {
        let want_to_turn = rng.bool(turn_float);
        let mut directions_to_try: Vec<Direction> = Direction::iterator().copied().collect();
        rng.shuffle(directions_to_try.as_mut_slice());
        if self.nodes.len() > 1 && !want_to_turn {
            directions_to_try.insert(0, self.current_dir);
        }
        directions_to_try
    }

    ///Portion of update logic related to determining the next
    /// node position of a pipe.
    fn get_next_pos(
        &mut self,
        directions_to_try: Vec<Direction>,
        occupied_nodes: &mut HashSet<Coordinate>,
    ) -> Option<Coordinate> {
        for dir in directions_to_try {
            let new_position = step_in_dir(self.get_current_head(), dir);
            if !is_in_bounds(new_position, self.space_bounds) {
                continue;
            }
            if occupied_nodes.contains(&new_position) {
                continue;
            }
            self.current_dir = dir;
            return Some(new_position);
        }
        None
    }
}

//=============================================
// World Implementation
//=============================================

///Used to give information to the render loop while
///minimizing the amount of state entanglement with
/// the world
pub(crate) struct PipeChangeData {
    pub last_node: Coordinate,
    pub last_dir: Direction,
    pub current_node: Coordinate,
    pub current_dir: Direction,
}

pub(crate) struct NewPipeData {
    pub start_node: Coordinate,
}

pub(crate) struct World {
    pipes: Vec<Pipe>,
    occupied_nodes: HashSet<Coordinate>,
    space_bounds: Coordinate,
    new_pipe_chance: f64,
    active_pipes: usize,
    gen_complete: bool,
    turn_chance: f32,
}

impl Default for World {
    fn default() -> Self {
        World {
            pipes: Vec::new(),
            occupied_nodes: HashSet::new(),
            space_bounds: (20, 20, 20),
            new_pipe_chance: 0.1,
            active_pipes: 0,
            gen_complete: false,
            turn_chance: 0.3,
        }
    }
}

impl World {
    pub fn new(config: Option<&Configuration>) -> Self {
        match config {
            Some(config) => World {
                space_bounds: (
                    config.world.max_bounds.0 as i32,
                    config.world.max_bounds.1 as i32,
                    config.world.max_bounds.2 as i32,
                ),
                turn_chance: config.world.turn_chance,
                ..Default::default()
            },
            None => World::default(),
        }
    }

    pub fn new_pipe(&mut self, rng: &mut impl EngineRng) -> NewPipeData {
        self.pipes
            .push(Pipe::new(&mut self.occupied_nodes, self.space_bounds, rng));
        let start = self.pipes[self.pipes.len() - 1].get_current_head();
        self.active_pipes += 1;

        NewPipeData { start_node: start }
    }

    pub fn pipe_update(&mut self, idx: usize, rng: &mut impl EngineRng) -> PipeChangeData {
        let last_node = self.pipes[idx].get_current_head();
        let last_dir = self.pipes[idx].get_current_dir();
        self.pipes[idx].update(&mut self.occupied_nodes, rng, Some(self.turn_chance));

        let current_node = self.pipes[idx].get_current_head();
        let current_dir = self.pipes[idx].get_current_dir();

        //Add a random chance post update to kill the pipe
        //increases the more the space is filled
        let total_nodes = self.space_bounds.0 * self.space_bounds.1 * self.space_bounds.2;
        let chance_to_kill = (self.occupied_nodes.len() as f64) / (total_nodes as f64);

        if self.pipes[idx].len() >= (total_nodes * 10 / 100) && rng.bool(chance_to_kill) {
            self.pipes[idx].kill();
            // self.active_pipes -= 1;
        }

        PipeChangeData {
            last_node,
            last_dir,
            current_node,
            current_dir,
        }
    }

    //=======================================
    // Encapsulation Things
    //=======================================
    pub fn active_pipes_count(&self) -> usize {
        self.active_pipes
    }

    pub fn max_active_count_reached(&self, max_num: u8) -> bool {
        self.active_pipes < max_num as usize
    }

    // pub fn max_active_pipe_idx(&self) -> usize {
    //     self.active_pipes - 1
    // }

    pub fn new_pipe_chance(&self) -> f64 {
        self.new_pipe_chance
    }

    pub fn is_pipe_alive(&self, idx: usize) -> bool {
        self.pipes[idx].is_alive()
    }

    pub fn is_gen_complete(&self) -> bool {
        self.gen_complete
    }

    pub fn set_gen_complete(&mut self) {
        self.gen_complete = true;
    }

    pub fn kill_pipe(&mut self, idx: usize) {
        self.pipes[idx].kill();
    }
}

//=============================================
// Unit Tests
//=============================================
#[cfg(test)]
mod tests {
    use mockall::{predicate, Sequence};

    use crate::engine::rng::{MockEngineRng, StdRng};

    use super::*;

    #[test]
    fn test_pipe_new_init() {
        let mut occupied_nodes: HashSet<Coordinate> = HashSet::new();
        let mut mock_rng = MockEngineRng::new();
        let mut seq = Sequence::new();
        let bounds = (2, 2, 2);

        mock_rng
            .expect_u8()
            .times(1)
            .return_const(2)
            .in_sequence(&mut seq);

        mock_rng
            .expect_i32()
            .times(3)
            .return_const(1)
            .in_sequence(&mut seq);

        let pipe = Pipe::new(&mut occupied_nodes, bounds, &mut mock_rng);

        assert!(pipe.is_alive());
        assert!(pipe.get_current_head() == (1, 1, 1));
        assert!(pipe.get_current_dir() == Direction::East);
        assert!(occupied_nodes.contains(&(1, 1, 1)));
    }

    #[test]
    fn test_pipe_getter_funcs() {
        let pipe = Pipe {
            alive: false,
            nodes: Vec::from(&[(1, 0, 0), (0, 0, 0), (0, 1, 0), (0, 1, 1)]),
            current_dir: Direction::Down,
            space_bounds: (2, 2, 2),
        };

        assert!(pipe.len() == 4);
        assert!(pipe.get_current_dir() == Direction::Down);
        assert!(pipe.get_current_head() == (1, 0, 0));
        assert!(!pipe.is_alive());
    }

    #[test]
    fn test_dead_pipe_does_not_update() {
        let mut occupied_nodes: HashSet<Coordinate> = HashSet::new();
        let mut rng = StdRng::new();
        let bounds = (10, 10, 10);
        let mut pipe = Pipe::new(&mut occupied_nodes, bounds, &mut rng);
        let head = pipe.get_current_head();
        assert!(pipe.is_alive());

        pipe.kill();
        assert!(!pipe.is_alive());
        //It should not matter what the occupied nodes are,
        //since the pipe is dead and should short circuit immediately
        pipe.update(&mut occupied_nodes, &mut rng, None);
        assert_eq!(pipe.get_current_head(), head);
    }

    #[test]
    fn test_pipe_update_get_directions() {
        //Part 1
        let mut pipe = Pipe {
            alive: true,
            current_dir: Direction::North,
            nodes: Vec::new(),
            space_bounds: (2, 2, 2),
        };
        let mut rng1 = MockEngineRng::new();
        let mut seq1 = Sequence::new();

        rng1.expect_bool()
            .once()
            .return_const(true)
            .in_sequence(&mut seq1);
        rng1.expect_shuffle::<Direction>()
            .once()
            .return_const(())
            .in_sequence(&mut seq1);

        let dirs1: Vec<Direction> = Direction::iterator().copied().collect();
        assert_eq!(pipe.get_directions(&mut rng1, 0.5), dirs1);

        //Part 2
        pipe.nodes.push((1, 1, 1));
        pipe.nodes.push((1, 1, 0));
        let mut rng2 = MockEngineRng::new();
        let mut seq2 = Sequence::new();

        rng2.expect_bool()
            .once()
            .return_const(true)
            .in_sequence(&mut seq2);
        rng2.expect_shuffle::<Direction>()
            .once()
            .return_const(())
            .in_sequence(&mut seq2);

        let dirs2: Vec<Direction> = Direction::iterator().copied().collect();
        assert_eq!(pipe.get_directions(&mut rng2, 0.5), dirs2);

        //Part 3
        let mut rng3 = MockEngineRng::new();
        let mut seq3 = Sequence::new();

        rng3.expect_bool()
            .once()
            .return_const(false)
            .in_sequence(&mut seq3);
        rng3.expect_shuffle::<Direction>()
            .once()
            .return_const(())
            .in_sequence(&mut seq3);

        let mut dirs3: Vec<Direction> = Direction::iterator().copied().collect();
        dirs3.insert(0, Direction::North);
        assert_eq!(pipe.get_directions(&mut rng3, 0.5), dirs3);
    }

    #[test]
    fn test_pipe_update_get_next_pos_satisfies_conds() {
        let mut pipe = Pipe {
            alive: true,
            current_dir: Direction::North,
            nodes: Vec::new(),
            space_bounds: (2, 2, 2),
        };
        let mut occupied: HashSet<Coordinate> = HashSet::new();
        pipe.nodes.push((1, 0, 0));
        pipe.nodes.push((1, 1, 0));
        occupied.insert((1, 0, 0));
        occupied.insert((1, 1, 0));

        //First Dir will be out of bounds, second dir will be occupied, third will satisfy conditions
        let dirs_to_try = Vec::from(&[Direction::North, Direction::Up, Direction::East]);
        let next_pos = pipe.get_next_pos(dirs_to_try, &mut occupied);

        assert!(next_pos == Some((1, 0, 1)));
        assert!(pipe.current_dir == Direction::East);
    }

    #[test]
    fn test_pipe_update_get_next_pos_fallback_conds() {
        let mut pipe = Pipe {
            alive: true,
            current_dir: Direction::North,
            nodes: Vec::new(),
            space_bounds: (2, 2, 2),
        };
        let mut occupied: HashSet<Coordinate> = HashSet::new();
        pipe.nodes.push((1, 0, 0));
        pipe.nodes.push((1, 1, 0));
        occupied.insert((1, 0, 0));
        occupied.insert((1, 1, 0));

        //No dirs will be usable, so we should get None back
        let dirs_to_try = Vec::from(&[Direction::North, Direction::Up]);
        let next_pos = pipe.get_next_pos(dirs_to_try, &mut occupied);

        assert!(next_pos == None);
        assert!(pipe.current_dir == Direction::North);
    }

    ///Two-fold test to check if the default or custom turn chance is used when provided
    #[test]
    fn test_pipe_update_provided_chance_used() {
        let mut pipe = Pipe {
            alive: true,
            current_dir: Direction::North,
            nodes: Vec::new(),
            space_bounds: (2, 2, 2),
        };
        let mut occupied: HashSet<Coordinate> = HashSet::new();
        pipe.nodes.push((1, 0, 0));
        pipe.nodes.push((1, 1, 0));
        occupied.insert((1, 0, 0));
        occupied.insert((1, 1, 0));

        let mut rng1 = MockEngineRng::new();
        rng1.expect_bool()
            .with(predicate::eq(1.0 / 2.0))
            .return_const(false);
        rng1.expect_shuffle::<Direction>().return_const(());
        pipe.update(&mut occupied, &mut rng1, None);

        let mut rng2 = MockEngineRng::new();
        rng2.expect_bool()
            .with(predicate::eq(0.25))
            .return_const(false);
        rng2.expect_shuffle::<Direction>().return_const(());
        pipe.update(&mut occupied, &mut rng2, Some(0.25));
    }

    #[test]
    fn test_pipe_update_kills_on_no_possible_dirs_used() {
        let mut pipe = Pipe {
            alive: true,
            current_dir: Direction::North,
            nodes: Vec::new(),
            space_bounds: (2, 2, 2),
        };
        let mut occupied: HashSet<Coordinate> = HashSet::new();
        pipe.nodes.push((1, 0, 0));
        pipe.nodes.push((1, 1, 0));
        occupied.insert((1, 0, 0));
        occupied.insert((1, 1, 0));
        occupied.insert((0, 0, 0));
        occupied.insert((1, 0, 1));

        let mut rng1 = MockEngineRng::new();
        rng1.expect_bool()
            .with(predicate::eq(1.0 / 2.0))
            .return_const(false);
        rng1.expect_shuffle::<Direction>().return_const(());
        pipe.update(&mut occupied, &mut rng1, None);

        assert!(!pipe.alive);
    }
}
