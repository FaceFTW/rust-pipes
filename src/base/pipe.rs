use rand::Rng;

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

    pub fn generate_pipe(&mut self) {
        let mut pipe = Pipe::new();

        dbg!("Generating pipe...");
        let mut current_node = self.nodes.find_random_empty_node(); //Starting Point
        let mut current_dir = self.nodes.find_random_direction(current_node).unwrap(); //Starting Direction
        let mut rng = rand::thread_rng();
        let max_turns = rng.gen_range(5..10);

        let mut current_turns = 0;
        let mut turn_weight = 0.1;

        pipe.add_node(current_node);
        pipe.add_sphere_point(current_node);

        //Main pipe-layer loop
        loop {
            dbg!(current_node);
            if current_turns >= max_turns {
                break;
            }

            //check if we *have* to turn (no more in direction)
            let open_nodes_in_dir = self
                .nodes
                .count_available_in_direction(current_node, current_dir);
            if open_nodes_in_dir == 0 {
                // dbg!("not enough in current dir");
                current_dir = match self.nodes.find_random_direction(current_node) {
                    Ok(dir) => dir,
                    Err(_) => break,
                };
                current_turns += 1;
                turn_weight = 0.1;
                pipe.add_sphere_point(current_node);
            } else {
                //check if we *should* turn (random chance)
                let should_turn = rng.gen_bool(turn_weight);
                if should_turn {
                    current_dir = match self.nodes.find_random_direction(current_node) {
                        Ok(dir) => dir,
                        Err(_) => current_dir, //If we can't find a new direction, just keep going
                    };
                    current_turns += 1;
                    turn_weight = 0.1;
                    pipe.add_sphere_point(current_node);
                }
            }

            //step in direction + add to pipe
            self.nodes.set(current_node);
            current_node = step_in_dir(current_node, current_dir);
            pipe.add_node(current_node);
            turn_weight += 0.1;
        }

        self.pipes.push(pipe);
    }
}
