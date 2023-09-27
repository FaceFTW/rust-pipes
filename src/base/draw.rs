use super::util::Coordinate;
use kiss3d::{
    nalgebra::{Point3, Translation3, UnitQuaternion, Vector3},
    scene::SceneNode,
    window::Window,
};

const PIPE_RADIUS: f32 = 0.5;
const PIPE_SEG_HEIGHT: f32 = 1.0;
const BALL_JOINT_RADIUS: f32 = 0.85;

pub fn make_pipe_section(
    from: Coordinate,
    to: Coordinate,
    window_ref: &mut Window,
    color: (f32, f32, f32),
) -> SceneNode {
    let mut pipe_section = window_ref.add_cylinder(PIPE_RADIUS, PIPE_SEG_HEIGHT);
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
        UnitQuaternion::rotation_between(&from_point, &(to_point - from_point)).unwrap(),
    );
    pipe_section
}

pub fn make_ball_joint(
    point: Coordinate,
    window_ref: &mut Window,
    color: (f32, f32, f32),
) -> SceneNode {
    let mut ball_joint = window_ref.add_sphere(BALL_JOINT_RADIUS);
    let (red, green, blue) = color;
    ball_joint.set_color(red, green, blue);
    ball_joint.set_local_translation(Translation3::new(
        point.0 as f32,
        point.1 as f32,
        point.2 as f32,
    ));
    ball_joint
}

///Method for drawing coordinate axes in "debug" modes
pub fn draw_axes(window: &mut Window) {
    let origin = Point3::<f32>::origin();
    let x_max = Point3::<f32>::new(30.0, 0.0, 0.0);
    let y_max = Point3::<f32>::new(0.0, 30.0, 0.0);
    let z_max = Point3::<f32>::new(0.0, 0.0, 30.0);

    let red = Point3::<f32>::new(1.0, 0.0, 0.0);
    let green = Point3::<f32>::new(0.0, 1.0, 0.0);
    let blue = Point3::<f32>::new(0.0, 0.0, 1.0);

    window.draw_line(&origin, &x_max, &red);
    window.draw_line(&origin, &y_max, &green);
    window.draw_line(&origin, &z_max, &blue);
}
