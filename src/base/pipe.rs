use rand::Rng;
use std::slice::Iter;

use super::node_struct::NodeStruct;
use super::util::*;

/// Represents a pipe to be rendered. This is a ordered
/// list of nodes that the pipe occupies. Other properties
/// related to the pipe (such as color) are not stored here.
pub struct Pipe {
    nodes: Vec<Coordinate>,
    sphere_points: Vec<Coordinate>,
}

impl Pipe {
    pub fn new() -> Pipe {
        Pipe {
            nodes: Vec::new(),
            sphere_points: Vec::new(),
        }
    }

    pub fn add_node(&mut self, node: Coordinate) {
        self.nodes.push(node);
    }

    pub fn add_sphere_point(&mut self, point: Coordinate) {
        self.sphere_points.push(point);
    }

    pub fn get_nodes(&self) -> &Vec<Coordinate> {
        &self.nodes
    }

    pub fn get_sphere_points(&self) -> &Vec<Coordinate> {
        &self.sphere_points
    }

    pub fn occupies_node(&self, node: Coordinate) -> bool {
        self.nodes.contains(&node)
    }

    pub fn tail(&self) -> Coordinate {
        self.nodes[self.nodes.len() - 1]
    }
}

/// Manages the pipes to be rendered. This controls the creation
/// of new pipes, but does not (currently) control the rendering
/// of the pipes.
pub struct PipeManager {
    pipes: Vec<Pipe>,
    space_size: Coordinate,
    nodes: NodeStruct,
}

impl PipeManager {
    pub fn new(size: Coordinate) -> PipeManager {
        PipeManager {
            pipes: Vec::new(),
            space_size: size,
            nodes: NodeStruct::new(size),
        }
    }

    pub fn get_pipes(&self) -> &Vec<Pipe> {
        &self.pipes
    }

    pub fn pipe(&self, index: usize) -> &Pipe {
        &self.pipes[index]
    }

    ///Recursive iteration for each step in a pipe
    /// generation routine.
    // fn pipe_gen_step(&mut self, &mut pipe: Pipe) {
    //     let nodes = *self.nodes.lock().unwrap();
    // 	let mut possible_nodes = Vec::new();
    // }

    /// Generates a new pipe. This is done by randomly
    /// selecting a node, and then randomly selecting
    /// a direction to go in. The pipe will then continue
    /// in that direction until either it reaches the
    /// edge of the space, it reaches a node that is
    /// already occupied, or it reaches a randomly determined maximum
    /// length.
    ///
    // pub fn generate_pipe(&mut self, mut pipe: Pipe) -> Pipe {
    //     let mut start_node = self.nodes.find_random_empty_node();
    //     let mut current_dir =
    //     pipe.add_node(start_node);
    //     pipe.add_sphere_point(start_node);

    //     let mut current_node = start_node;

    //     let max_iter = rand::thread_rng().gen_range(5..10);
    //     let mut iter = 0;
    //     while iter < max_iter {
    //         let open_nodes_in_dir = self
    //             .nodes
    //             .count_available_in_direction(current_node, Direction::North);

    //         let pipe_length = rand::thread_rng().gen_range(1..open_nodes_in_dir);

    //         let mut pipe_iter = 0;
    //         while pipe_iter < pipe_length - 1 {
    //             current_node = step_in_dir(current_node, Direction::North);
    //             pipe.add_node(current_node);
    //             pipe_iter += 1;
    //         }

    //         start_node = step_in_dir(current_node, dir);

    //         iter += 1;
    //     }

    //     return pipe;
    // }

    pub fn generate_pipe(&mut self) {
        let mut pipe = Pipe::new();

        let mut current_node = self.nodes.find_random_empty_node(); //Starting Point
        let mut current_dir = self.nodes.find_random_direction(current_node); //Starting Direction
        let mut rng = rand::thread_rng();
        let max_turns = rng.gen_range(5..10);

        let mut current_turns = 0;
        let mut turn_weight = 0.1;

        pipe.add_node(current_node);
        pipe.add_sphere_point(current_node);

        //Main pipe-layer loop
        loop {
            if current_turns >= max_turns {
                break;
            }

            //check if we *have* to turn (no more in direction)
            let open_nodes_in_dir = self
                .nodes
                .count_available_in_direction(current_node, current_dir);
            if open_nodes_in_dir == 0 {
                current_dir = self.nodes.find_random_direction(current_node);
                current_turns += 1;
                turn_weight = 0.1;
                pipe.add_sphere_point(current_node);
            } else {
                //check if we *should* turn (random chance)
                let should_turn = rng.gen_bool(turn_weight);
                if should_turn {
                    current_dir = self.nodes.find_random_direction(current_node);
                    current_turns += 1;
                    turn_weight = 0.1;
                    pipe.add_sphere_point(current_node);
                }
            }

            //step in direction + add to pipe
            current_node = step_in_dir(current_node, current_dir);
			pipe.add_node(current_node);
            turn_weight += 0.1;
        }

        self.pipes.push(pipe);
    }
}
