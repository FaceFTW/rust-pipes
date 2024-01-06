///Defines core functions and structs used in the main engine loop

use three_d::Instances;

use super::{
    rng::EngineRng,
    util::{make_instanced_ball_joint, make_instanced_pipe_section, InstantShim},
    world::World, config::Configuration,
};

pub(crate) fn update_pipe_step(
    world: &mut World,
    rng: &mut impl EngineRng,
    ball_instances: &mut Instances,
    pipe_instances: &mut Instances,
) {
    for i in 0..world.active_pipes_count() {
        if !world.is_pipe_alive(i) {
            continue;
        }
        let delta_state = world.pipe_update(i, rng);

        if delta_state.last_node != delta_state.current_node {
            if delta_state.last_dir != delta_state.current_dir {
                make_instanced_ball_joint(
                    ball_instances,
                    delta_state.last_node,
                    delta_state.pipe_color,
                );
            }
            make_instanced_pipe_section(
                pipe_instances,
                delta_state.last_node,
                delta_state.current_node,
                delta_state.pipe_color,
            );
        }
    }
}

pub(crate) fn new_pipe_step(
    rng: &mut impl EngineRng,
    world: &mut World,
    cfg: &Configuration,
    ball_instances: &mut Instances,
) {
    if rng.bool(world.new_pipe_chance()) && world.max_active_count_reached(cfg.world.max_pipes) {
        let data = world.new_pipe(rng);
        make_instanced_ball_joint(ball_instances, data.start_node, data.color);
    }
}

pub(crate) fn check_time_step(
    start_time: InstantShim,
    cfg: &Configuration,
    world: &mut World,
) -> bool {
    match start_time.elapsed() {
        elapsed if elapsed.as_secs_f64() < cfg.world.max_gen_time as f64 => (),
        elapsed
            if elapsed.as_secs_f64() >= cfg.world.max_gen_time as f64
                && elapsed.as_secs_f64() < cfg.world.max_freeze_time as f64 =>
        {
            world.set_gen_complete();
            for i in 0..world.active_pipes_count() {
                world.kill_pipe(i);
            }
        }
        elapsed
            if elapsed.as_secs_f64()
                >= (cfg.world.max_gen_time + cfg.world.max_freeze_time) as f64
                && elapsed.as_secs_f64() < cfg.world.max_cycle_time =>
        {
            ()
        }
        _ => return true,
    }
    false
}
