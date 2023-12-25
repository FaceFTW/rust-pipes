use crate::config::Configuration;
use crate::util::*;
use rand::{seq::SliceRandom, Rng};
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
        rng: &mut impl Rng,
    ) -> Pipe {
        let mut new_pipe: Pipe = Pipe {
            alive: true,
            nodes: Vec::new(),
            current_dir: choose_random_direction(rng),
            space_bounds: bounds,
        };

        let start = find_random_start(&occupied_nodes, bounds, rng);
        new_pipe.nodes.push(start);
        occupied_nodes.insert(start);

        return new_pipe;
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
        rng: &mut impl Rng,
        turn_chance: Option<f32>,
    ) {
        if !self.alive {
            return;
        }

        let turn_float: f64 = match turn_chance {
            Some(chance) => chance.into(),
            None => 1.0 / 2.0,
        };

        let want_to_turn = rng.gen_bool(turn_float);
        let mut directions_to_try: Vec<Direction> =
            Direction::iterator().map(|dirs| *dirs).collect();
        directions_to_try.shuffle(rng);
        if self.nodes.len() > 1 && !want_to_turn {
            directions_to_try.insert(0, self.current_dir);
        }

        let mut found_valid_direction = false;
        let mut new_position: Coordinate = (0, 0, 0);
        for dir in directions_to_try {
            new_position = step_in_dir(self.get_current_head(), dir);
            if !is_in_bounds(new_position, self.space_bounds) {
                continue;
            }
            if occupied_nodes.contains(&new_position) {
                continue;
            }
            found_valid_direction = true;
            self.current_dir = dir;
            break;
        }
        if !found_valid_direction {
            self.kill();
            return;
        }

        occupied_nodes.insert(new_position);
        self.nodes.insert(0, new_position);
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
    pub pipe_color: Color,
}

pub(crate) struct NewPipeData {
    pub start_node: Coordinate,
    pub color: Color,
}

pub(crate) struct World {
    pipes: Vec<Pipe>,
    pipe_colors: Vec<Color>,
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
            pipe_colors: Vec::new(),
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

    pub fn new_pipe(&mut self, rng: &mut impl Rng) -> NewPipeData {
        self.pipes
            .push(Pipe::new(&mut self.occupied_nodes, self.space_bounds, rng));
        let start = self.pipes[self.pipes.len() - 1].get_current_head();
        let new_color: Color = rng.gen();
        self.pipe_colors.push(new_color);
        self.active_pipes += 1;

        NewPipeData {
            start_node: start,
            color: new_color,
        }
    }

    pub fn pipe_update(&mut self, idx: usize, rng: &mut impl Rng) -> PipeChangeData {
        let color = self.pipe_colors[idx];
        let last_node = self.pipes[idx].get_current_head();
        let last_dir = self.pipes[idx].get_current_dir();
        self.pipes[idx].update(&mut self.occupied_nodes, rng, Some(self.turn_chance));

        let current_node = self.pipes[idx].get_current_head();
        let current_dir = self.pipes[idx].get_current_dir();

        //Add a random chance post update to kill the pipe
        //increases the more the space is filled
        let total_nodes = self.space_bounds.0 * self.space_bounds.1 * self.space_bounds.2;
        let chance_to_kill = (self.occupied_nodes.len() as f64) / (total_nodes as f64);

        if self.pipes[idx].len() >= (total_nodes * 10 / 100) as i32 && rng.gen_bool(chance_to_kill)
        {
            self.pipes[idx].kill();
            // self.active_pipes -= 1;
        }

        PipeChangeData {
            last_node: last_node,
            last_dir: last_dir,
            current_node: current_node,
            current_dir: current_dir,
            pipe_color: color,
        }
    }

    //=======================================
    // Encapsulation Things
    //=======================================
    pub fn active_pipes_count(&self) -> usize {
        self.active_pipes
    }

    pub fn max_active_count_reached(&self, max_num: u32) -> bool {
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
    use super::*;

    #[test]
    fn test_dead_pipe_does_not_update() {
        let mut occupied_nodes: HashSet<Coordinate> = HashSet::new();
        let mut rng = rand::thread_rng();
        let bounds = (10, 10, 10);
        let mut pipe = Pipe::new(&mut occupied_nodes, bounds, &mut rng);
        let head = pipe.get_current_head();
        pipe.kill();

        //It should not matter what the occupied nodes are,
        //since the pipe is dead and should short circuit immediately
        pipe.update(&mut occupied_nodes, &mut rng, None);
        assert_eq!(pipe.get_current_head(), head);
    }
}
