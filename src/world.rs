use std::collections::HashSet;

use rand::Rng;

use crate::config::Configuration;

use super::{
    pipe::Pipe,
    util::{Color, Coordinate, Direction},
};

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
