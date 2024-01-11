mod config;
mod core;
mod rng;
mod util;
mod world;

use crate::engine::{
    config::Configuration,
    rng::{EngineRng, StdRng},
    util::InstantShim,
    world::World,
};
use cfg_if::cfg_if;
use three_d::{
    degrees, vec3, Camera, ClearState, CpuMaterial, CpuMesh, DirectionalLight, FrameOutput, Gm,
    InstancedMesh, Instances, OrbitControl, PhysicalMaterial, Srgba, Window, WindowSettings,Context
};

use self::core::Engine;

pub fn real_main() {
    //===============================================
    // WORLD INITIALIZATION
    //===============================================
    let cfg: Configuration = get_config();
    // let mut world = World::new(Some(&cfg));
    // let mut rng = match cfg.rng_seed {
    //     Some(seed) => StdRng::with_seed(seed),
    //     None => StdRng::new(),
    // };
    // // dbg!(rng.get_seed());

    //===============================================
    // ENGINE INIT
    //===============================================
    let window = Window::new(WindowSettings {
        title: "Rust Pipes".to_string(),
        max_size: cfg.window_size,
        ..Default::default()
    })
    .unwrap();
    let context:Context = window.gl();

    let mut camera = Camera::new_perspective(
        window.viewport(),
        vec3(-20.0, 10.0, -20.0),
        vec3(0.0, 10.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        degrees(45.0),
        0.1,
        1000.0,
    );
    let mut control = OrbitControl::new(*camera.target(), 1.0, 100.0);

    let light0 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, -0.5, -0.5));
    let light1 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, 0.5, 0.5));

    // //Instanced Rendering Data
    // let mut pipe_instances = Instances {
    //     transformations: Vec::new(),
    //     colors: Some(Vec::new()),
    //     ..Default::default()
    // };
    // let mut ball_instances = Instances {
    //     transformations: Vec::new(),
    //     colors: Some(Vec::new()),
    //     ..Default::default()
    // };

    // let base_instance_material = CpuMaterial {
    //     albedo: Srgba {
    //         r: 255,
    //         g: 255,
    //         b: 255,
    //         a: 255,
    //     },
    //     ..Default::default()
    // };

    // let mut pipe_instance_mesh = Gm::new(
    //     InstancedMesh::new(
    //         &context,
    //         &Instances::default(),
    //         &CpuMesh::cylinder(cfg.draw.angle_subdiv),
    //     ),
    //     PhysicalMaterial::new(&context, &base_instance_material),
    // );
    // let mut ball_instance_mesh = Gm::new(
    //     InstancedMesh::new(
    //         &context,
    //         &Instances::default(),
    //         &CpuMesh::sphere(cfg.draw.angle_subdiv),
    //     ),
    //     PhysicalMaterial::new(&context, &base_instance_material),
    // );

    let mut start_time = InstantShim::now();
    let mut do_reset = false;

    let mut engine = Engine::new(&cfg, &context);
    window.render_loop(move |mut frame_input| {
        camera.set_viewport(frame_input.viewport);
        control.handle_events(&mut camera, &mut frame_input.events);

        if do_reset && !cfg.single_run {
            start_time = InstantShim::now();
            engine.reset_state();
            // world = World::new(Some(&cfg));
            // pipe_instances = Instances {
            //     transformations: Vec::new(),
            //     colors: Some(Vec::new()),
            //     ..Default::default()
            // };
            // ball_instances = Instances {
            //     transformations: Vec::new(),
            //     colors: Some(Vec::new()),
            //     ..Default::default()
            // };
            // // do_reset = false;
        }

        //World Update Step
        do_reset = engine.update_state(start_time);
        // do_reset = world_update_tick(
        //     &mut world,
        //     &mut rng,
        //     &mut ball_instances,
        //     &mut pipe_instances,
        //     start_time,
        //     &cfg,
        // );

        // pipe_instance_mesh.set_instances(&pipe_instances);
        // ball_instance_mesh.set_instances(&ball_instances);
        frame_input
            .screen()
            .clear(ClearState::color_and_depth(
                cfg.draw.bg_color.0 as f32 / 255.0,
                cfg.draw.bg_color.1 as f32 / 255.0,
                cfg.draw.bg_color.2 as f32 / 255.0,
                1.0,
                1.0,
            ))
            .render(&camera, engine.get_render_objects(), &[&light0, &light1]);

        FrameOutput::default()
    });
}

cfg_if! {
  if #[cfg(target_arch="wasm32")]{
    fn get_config() -> Configuration{
        Configuration::new()
    }
  } else {
    use crate::engine::config::make_cli_parser;
    fn get_config() -> Configuration{
        let cli_args = &make_cli_parser().get_matches();
        // dbg!(cli_args);
        Configuration::new(cli_args)
    }
  }
}

fn world_update_tick(
    world: &mut World,
    rng: &mut impl EngineRng,
    ball_instances: &mut Instances,
    pipe_instances: &mut Instances,
    start_time: InstantShim,
    cfg: &Configuration,
) -> bool {
    if !world.is_gen_complete() {
        core::update_pipe_step(world, rng, ball_instances, pipe_instances);
    }

    core::new_pipe_step(rng, world, cfg, ball_instances);

    core::check_time_step(start_time, cfg, world)
}

// #[cfg(test)]
// mod tests {
//     use super::*;
// 	pub fn test(){

// 	}
// }
