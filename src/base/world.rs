use std::collections::HashSet;

use rand::Rng;

use super::{
    pipe::Pipe,
    util::{Color, Coordinate, Direction},
};

///Used to give information to the render loop while
///minimizing the amount of state entanglement with
/// the world
pub struct PipeChange {
    pub last_node: Coordinate,
    pub last_dir: Direction,
    pub current_node: Coordinate,
    pub current_dir: Direction,
    pub pipe_color: Color,
}

pub struct NewPipeData {
    pub start_node: Coordinate,
    pub color: Color,
}

pub struct World {
    pub pipes: Vec<Pipe>,
    pub pipe_colors: Vec<Color>,
    occupied_nodes: HashSet<Coordinate>,
    space_bounds: Coordinate,
    pub new_pipe_chance: f64,
    pub active_pipes: usize,
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
        }
    }
}

impl World {
    pub fn new() -> Self {
        World {
            ..Default::default()
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

    pub fn pipe_update(&mut self, idx: usize, rng: &mut impl Rng) -> PipeChange {
        let color = self.pipe_colors[idx];
        let last_node = self.pipes[idx].get_current_head();
        let last_dir = self.pipes[idx].get_current_dir();
        self.pipes[idx].update(&mut self.occupied_nodes, rng);

        let current_node = self.pipes[idx].get_current_head();
        let current_dir = self.pipes[idx].get_current_dir();

        PipeChange {
            last_node: last_node,
            last_dir: last_dir,
            current_node: current_node,
            current_dir: current_dir,
            pipe_color: color,
        }
    }
}
