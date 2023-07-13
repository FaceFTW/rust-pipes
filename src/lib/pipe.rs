use super::util::*;

/// Represents a pipe to be rendered. This is a ordered
/// list of nodes that the pipe occupies. Other properties
/// related to the pipe (such as color) are not stored here.
pub struct Pipe {
    nodes: Vec<Coordinate>,
}

impl Pipe {
    fn new() -> Pipe {
        Pipe { nodes: Vec::new() }
    }

    fn add_node(&mut self, node: Coordinate) {
        self.nodes.push(node);
    }

    fn get_nodes(&self) -> &Vec<Coordinate> {
        &self.nodes
    }

    fn occupies_node(&self, node: Coordinate) -> bool {
        self.nodes.contains(&node)
    }
}

/// Manages the pipes to be rendered. This controls the creation
/// of new pipes, but does not (currently) control the rendering
/// of the pipes.
struct PipeManager {
    pipes: Vec<Pipe>,
    space_length: i32,
    space_width: i32,
    space_height: i32,
    // occupied_nodes: [[[bool]]], // TODO: Replace with Vec<Vec<Vec<bool
}

impl PipeManager {
    fn new(len: i32, wid: i32, height: i32) -> PipeManager {
        PipeManager {
            pipes: Vec::new(),
            space_length: len,
            space_width: wid,
            space_height: height,
            // occupied_nodes: Vec::new(),
        }
    }

    fn get_pipes(&self) -> &Vec<Pipe> {
        &self.pipes
    }

    fn occupies_node(&self, node: Coordinate) -> bool {
        for pipe in &self.pipes {
            if pipe.occupies_node(node) {
                return true;
            }
        }
        false
    }
}
