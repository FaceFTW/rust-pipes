use std::collections::HashSet;

use rand::{seq::SliceRandom, Rng};

use super::util::*;

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

    pub(crate) fn kill(&mut self) {
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

    pub fn update(&mut self, occupied_nodes: &mut HashSet<Coordinate>, rng: &mut impl Rng) {
        if !self.alive {
            return;
        }

        let want_to_turn = rng.gen_bool(1.0 / 2.0);
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
        pipe.update(&mut occupied_nodes, &mut rng);
        assert_eq!(pipe.get_current_head(), head);
    }
}
