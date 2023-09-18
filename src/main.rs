use std::{collections::HashSet, os::windows::thread, thread::sleep, time::Duration};

use base::{
    draw::{make_ball_joint, make_pipe_section},
    pipe::Pipe,
    util::Coordinate,
};
use kiss3d::{camera::ArcBall, nalgebra::Point3, window::Window};
use rand::Rng;

extern crate kiss3d;

mod base;

struct World {
    pipes: Vec<Pipe>,
    pipe_node_vectors: Vec<Vec<kiss3d::scene::SceneNode>>,
    occupied_nodes: HashSet<Coordinate>,
    space_bounds: Coordinate,
    new_pipe_chance: f64,
    max_pipes: usize,
    active_pipes: usize,
}

impl Default for World {
    fn default() -> Self {
        World {
            pipes: Vec::new(),
            occupied_nodes: HashSet::new(),
            pipe_node_vectors: Vec::new(),
            space_bounds: (50, 50, 50),
            new_pipe_chance: 0.5,
            max_pipes: 10,
            active_pipes: 0,
        }
    }
}

fn main() {
    //===============================================
    // WORLD INITIALIZATION
    //===============================================
    let mut world = World {
        pipes: Vec::new(),
        pipe_node_vectors: Vec::new(),
        occupied_nodes: HashSet::new(),
        ..Default::default()
    };
    for _ in 0..world.max_pipes {
        world.pipe_node_vectors.push(Vec::new());
    }
    let mut rng = rand::thread_rng();
    world.pipes.push(Pipe::new(
        &mut world.occupied_nodes,
        world.space_bounds,
        &mut rng,
    ));
    world.active_pipes += 1;

    //===============================================
    // KISS3D INITIALIZATION
    //===============================================
    let mut window = Window::new("rust-pipes");
    let eye = Point3::new(-20.0 as f32, -20.0, -20.0);
    let at_point = Point3::new(0.0 as f32, 0.0, 0.0);
    let mut camera = ArcBall::new(eye, at_point);

    while window.render_with_camera(&mut camera) {
        sleep(Duration::from_millis(100));
        for i in 0..world.active_pipes {
            let lastNode = world.pipes[i].get_current_head();
            let lastDir = world.pipes[i].get_current_dir();
            world.pipes[i].update(&mut world.occupied_nodes, &mut rng);
            if !world.pipes[i].is_alive() {
                continue;
            }
            let currentNode = world.pipes[i].get_current_head();
            let currentDir = world.pipes[i].get_current_dir();
            if lastDir != currentDir {
                world.pipe_node_vectors[i].push(make_ball_joint(
                    lastNode,
                    &mut window,
                    (0.0, 1.0, 0.0),
                ));
            } else {
                world.pipe_node_vectors[i].push(make_pipe_section(
                    lastNode,
                    currentNode,
                    &mut window,
                    (0.0, 1.0, 0.0),
                ));
            }
        }
        if rng.gen_bool(world.new_pipe_chance) && world.active_pipes < world.max_pipes {
            world.pipes.push(Pipe::new(
                &mut world.occupied_nodes,
                world.space_bounds,
                &mut rng,
            ));
            world.active_pipes += 1;

            world.pipe_node_vectors[world.active_pipes - 1].push(make_ball_joint(
                world.pipes[world.active_pipes - 1].get_current_head(),
                &mut window,
                (0.0, 1.0, 0.0),
            ));
        }
    }
}

#[cfg(test)]
mod tests {}
