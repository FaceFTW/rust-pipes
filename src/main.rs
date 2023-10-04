use draw::{make_ball_joint, make_pipe_section, RenderObject};
use rand::Rng;
use three_d::{
    degrees, vec3, Axes, Camera, ClearState, DirectionalLight, FrameOutput, OrbitControl, Srgba,
    Window, WindowSettings,
};
use world::World;

mod draw;
mod pipe;
mod util;
mod world;

const MAX_PIPES: i32 = 10;

fn main() {
    //===============================================
    // WORLD INITIALIZATION
    //===============================================
    let mut world = World::new();
    let mut rng = rand::thread_rng();
    world.new_pipe(&mut rng);

    //===============================================
    // ENGINE INIT
    //===============================================
    let window = Window::new(WindowSettings {
        title: "Rust Pipes".to_string(),
        max_size: Some((1280, 720)),
        ..Default::default()
    })
    .unwrap();
    let context = window.gl();

    let mut camera = Camera::new_perspective(
        window.viewport(),
        vec3(5.0, 2.0, 2.5),
        vec3(0.0, 0.0, -0.5),
        vec3(0.0, 1.0, 0.0),
        degrees(45.0),
        0.1,
        1000.0,
    );
    let mut control = OrbitControl::new(*camera.target(), 1.0, 100.0);

    // let axes = Axes::new(&context, 0.1, 2.0);
    let light0 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, -0.5, -0.5));
    let light1 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, 0.5, 0.5));

    let mut pipe_render_objs: [Vec<RenderObject>; 10] = Default::default();

    window.render_loop(move |mut frame_input| {
        camera.set_viewport(frame_input.viewport);
        control.handle_events(&mut camera, &mut frame_input.events);

        //World Update Step
        for i in 0..world.active_pipes {
            if !world.pipes[i].is_alive() {
                continue;
            }
            let delta_state = world.pipe_update(i, &mut rng);

            if delta_state.last_node != delta_state.current_node {
                if delta_state.last_dir != delta_state.current_dir {
                    pipe_render_objs[i].push(make_ball_joint(
                        delta_state.last_node,
                        delta_state.pipe_color,
                        &context,
                    ));
                } else {
                    pipe_render_objs[i].push(make_pipe_section(
                        delta_state.last_node,
                        delta_state.current_node,
                        delta_state.pipe_color,
                        &context,
                    ));
                }
            }
        }
        if rng.gen_bool(world.new_pipe_chance) && world.active_pipes < MAX_PIPES as usize {
            let data = world.new_pipe(&mut rng);
            pipe_render_objs[world.active_pipes - 1].push(make_ball_joint(
                data.start_node,
                data.color,
                &context,
            ));
        }

        frame_input
            .screen()
            .clear(ClearState::color_and_depth(0.8, 0.8, 0.8, 1.0, 1.0))
            .render(
                &camera,
                pipe_render_objs.iter().flatten(),
                &[&light0, &light1],
            );

        FrameOutput::default()
    });
}

#[cfg(test)]
mod tests {}
