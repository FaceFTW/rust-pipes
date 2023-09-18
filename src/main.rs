use std::{collections::HashSet, thread::sleep, time::Duration};

use base::{
    draw::{make_ball_joint, make_pipe_section},
    pipe::Pipe,
    util::Coordinate,
};
use kiss3d::{camera::ArcBall, nalgebra::Point3, scene::SceneNode, window::Window};
use rand::Rng;

extern crate kiss3d;

mod base;

const MAX_PIPES: i32 = 10;

struct World {
    pipes: Vec<Pipe>,
    occupied_nodes: HashSet<Coordinate>,
    space_bounds: Coordinate,
    new_pipe_chance: f64,
    active_pipes: usize,
}

impl Default for World {
    fn default() -> Self {
        World {
            pipes: Vec::new(),
            occupied_nodes: HashSet::new(),
            space_bounds: (50, 50, 50),
            new_pipe_chance: 0.1,
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
        occupied_nodes: HashSet::new(),
        ..Default::default()
    };
    let mut pipe_nodes: [Vec<SceneNode>; MAX_PIPES as usize] = Default::default();
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
            let last_node = world.pipes[i].get_current_head();
            let last_dir = world.pipes[i].get_current_dir();
            world.pipes[i].update(&mut world.occupied_nodes, &mut rng);
            if !world.pipes[i].is_alive() {
                continue;
            }
            let current_node = world.pipes[i].get_current_head();
            let current_dir = world.pipes[i].get_current_dir();
            if last_dir != current_dir {
                pipe_nodes[i].push(make_ball_joint(last_node, &mut window, (0.0, 1.0, 0.0)));
            } else {
                pipe_nodes[i].push(make_pipe_section(
                    last_node,
                    current_node,
                    &mut window,
                    (0.0, 1.0, 0.0),
                ));
            }
        }
        if rng.gen_bool(world.new_pipe_chance) && world.active_pipes < MAX_PIPES as usize {
            world.pipes.push(Pipe::new(
                &mut world.occupied_nodes,
                world.space_bounds,
                &mut rng,
            ));
            world.active_pipes += 1;

            pipe_nodes[world.active_pipes - 1].push(make_ball_joint(
                world.pipes[world.active_pipes - 1].get_current_head(),
                &mut window,
                (0.0, 1.0, 0.0),
            ));
        }
    }
}

#[cfg(test)]
mod tests {}
