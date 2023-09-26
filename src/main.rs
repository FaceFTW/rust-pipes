use std::{collections::HashSet, thread::sleep, time::Duration};

use base::{
    draw::{make_ball_joint, make_pipe_section},
    pipe::Pipe,
    util::{Color, Coordinate},
};
use kiss3d::{camera::ArcBall, nalgebra::Point3, scene::SceneNode, window::Window};
use rand::Rng;

extern crate kiss3d;

mod base;

const MAX_PIPES: i32 = 10;

struct World {
    pipes: Vec<Pipe>,
    pipe_colors: Vec<Color>,
    occupied_nodes: HashSet<Coordinate>,
    space_bounds: Coordinate,
    new_pipe_chance: f64,
    active_pipes: usize,
}

impl Default for World {
    fn default() -> Self {
        World {
            pipes: Vec::new(),
            pipe_colors: Vec::new(),
            occupied_nodes: HashSet::new(),
            space_bounds: (25,25,25),
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
        ..Default::default()
    };
    let mut pipe_nodes: Vec<SceneNode> = Vec::new();
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
    let eye = Point3::new(-20.0 as f32, -20.0, 20.0);
    let at_point = Point3::new(25.0 as f32, 25.0, 25.0);
    let mut camera = ArcBall::new(eye, at_point);

    while window.render_with_camera(&mut camera) {
        sleep(Duration::from_millis(100));
        for i in 0..world.active_pipes {
            if world.pipe_colors.len() < i + 1 {
                let new_color: Color = rng.gen();
                world.pipe_colors.push(new_color);
            }
            let pipe_color = world.pipe_colors[i];
            let last_node = world.pipes[i].get_current_head();
            let last_dir = world.pipes[i].get_current_dir();
            world.pipes[i].update(&mut world.occupied_nodes, &mut rng);
            if !world.pipes[i].is_alive() {
                continue;
            }
            let current_node = world.pipes[i].get_current_head();
            let current_dir = world.pipes[i].get_current_dir();
            if last_dir != current_dir && last_node != current_node {
                pipe_nodes.push(make_ball_joint(last_node, &mut window, pipe_color));
            } else {
                if last_node != current_node {
                    pipe_nodes.push(make_pipe_section(
                        last_node,
                        current_node,
                        &mut window,
                        pipe_color,
                    ));
                }
            }
        }
        if rng.gen_bool(world.new_pipe_chance) && world.active_pipes < MAX_PIPES as usize {
            world.pipes.push(Pipe::new(
                &mut world.occupied_nodes,
                world.space_bounds,
                &mut rng,
            ));
            world.active_pipes += 1;

            let new_color: Color = rng.gen();
            world.pipe_colors.push(new_color);
            pipe_nodes.push(make_ball_joint(
                world.pipes[world.active_pipes - 1].get_current_head(),
                &mut window,
                new_color,
            ));
        }
    }
}

#[cfg(test)]
mod tests {}
