use kiss3d::{
    nalgebra::{Translation3, UnitQuaternion, Vector3},
    window::Window,
};

use super::util::Coordinate;
use kiss3d::scene::SceneNode;

pub fn make_pipe_section(
    from: Coordinate,
    to: Coordinate,
    window_ref: &mut Window,
    color: (f32, f32, f32),
) -> SceneNode {
    let mut pipe_section = window_ref.add_cylinder(0.5, 1.0);
    let from_point = Vector3::new(from.0 as f32, from.1 as f32, from.2 as f32);
    let to_point = Vector3::new(to.0 as f32, to.1 as f32, to.2 as f32);

    let (red, green, blue) = color;
    pipe_section.set_color(red, green, blue);
    pipe_section.set_local_translation(Translation3::new(
        from.0 as f32,
        from.1 as f32,
        from.2 as f32,
    ));
    pipe_section.set_local_rotation(
        UnitQuaternion::rotation_between(&Vector3::y(), &(to_point - from_point)).unwrap(),
    );
    pipe_section
}

pub fn make_ball_joint(
    point: Coordinate,
    window_ref: &mut Window,
    color: (f32, f32, f32),
) -> SceneNode {
    let mut ball_joint = window_ref.add_sphere(0.85);
    let (red, green, blue) = color;
    ball_joint.set_color(red, green, blue);
    ball_joint.set_local_translation(Translation3::new(
        point.0 as f32,
        point.1 as f32,
        point.2 as f32,
    ));
    ball_joint
}
