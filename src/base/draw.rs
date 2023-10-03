use super::util::Coordinate;

const PIPE_RADIUS: f32 = 0.5;
const PIPE_SEG_HEIGHT: f32 = 1.0;
const BALL_JOINT_RADIUS: f32 = 0.85;

#[cfg(feature = "k3d_engine")]
pub fn make_pipe_section(
    from: Coordinate,
    to: Coordinate,
    window_ref: &mut kiss3d::window::Window,
    color: (f32, f32, f32),
) -> kiss3d::scene::SceneNode {
    use kiss3d::{
        nalgebra::{Point3, Translation3, UnitQuaternion, Vector3},
        scene::SceneNode,
        window::Window,
    };
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
    pipe_section
        .set_local_rotation(UnitQuaternion::rotation_between(&from_point, &to_point).unwrap());
    pipe_section
}

#[cfg(feature = "k3d_engine")]
pub fn make_ball_joint(
    point: Coordinate,
    window_ref: &mut kiss3d::window::Window,
    color: (f32, f32, f32),
) -> kiss3d::scene::SceneNode {
    use kiss3d::{
        nalgebra::{Point3, Translation3, UnitQuaternion, Vector3},
        scene::SceneNode,
        window::Window,
    };
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
#[cfg(feature = "k3d_engine")]
pub fn draw_axes(window: &mut kiss3d::window::Window) {
    use kiss3d::{
        nalgebra::{Point3, Translation3, UnitQuaternion, Vector3},
        scene::SceneNode,
        window::Window,
    };
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

#[cfg(all(feature = "k3d_engine", test))]
mod tests {
    use std::f32::consts::PI;

    use kiss3d::{
        nalgebra::{Point3, UnitQuaternion, Vector3},
        window::Window,
    };

    //Here, we effectively check if the SceneNode is generated as expected
    use crate::base::util::{Color, Coordinate};

    use super::make_pipe_section;

    #[test]
    fn make_pipe_section_origin_start_no_translation() {
        let mut window = Window::new_hidden("test");
        let from: Coordinate = (0, 0, 0);
        let to: Coordinate = (1, 0, 0);
        let color: Color = (0.0, 1.0, 0.0);

        let node = make_pipe_section(from, to, &mut window, color);
        let node_data = node.data();

        assert_eq!(node_data.local_translation().vector, Vector3::zeros());
    }

    #[test]
    fn make_pipe_section_non_origin_start_has_translation() {
        let mut window = Window::new_hidden("test");
        let from: Coordinate = (1, 0, 0);
        let to: Coordinate = (2, 0, 0);
        let color: Color = (0.0, 1.0, 0.0);

        let node = make_pipe_section(from, to, &mut window, color);
        let node_data = node.data();

        assert_eq!(
            node_data.local_translation().vector,
            Vector3::new(1.0, 0.0, 0.0)
        );
    }

    #[test]
    fn make_pipe_section_north_dir_quarternion_applied() {
        let mut window = Window::new_hidden("test");
        let from: Coordinate = (0, 0, 0);
        let to: Coordinate = (1, 0, 0);
        let color: Color = (0.0, 1.0, 0.0);

        let node = make_pipe_section(from, to, &mut window, color);
        let node_data = node.data();

        let axisangle = Vector3::x() * 0.0;
        assert_eq!(
            node_data.local_transformation().rotation,
            UnitQuaternion::<f32>::new(axisangle)
        )
    }

    #[test]
    fn make_pipe_section_south_dir_quarternion_applied() {
        let mut window = Window::new_hidden("test");
        let from: Coordinate = (0, 0, 0);
        let to: Coordinate = (-1, 0, 0);
        let color: Color = (0.0, 1.0, 0.0);

        let node = make_pipe_section(from, to, &mut window, color);
        let node_data = node.data();

        let axisangle = Vector3::x() * PI;
        assert_eq!(
            node_data.local_transformation().rotation,
            UnitQuaternion::<f32>::new(axisangle)
        )
    }

    #[test]
    fn make_pipe_section_east_dir_quarternion_applied() {
        let mut window = Window::new_hidden("test");
        let from: Coordinate = (0, 0, 0);
        let to: Coordinate = (0, 0, 1);
        let color: Color = (0.0, 1.0, 0.0);

        let node = make_pipe_section(from, to, &mut window, color);
        let node_data = node.data();

        let axisangle = Vector3::x() * 0.0;
        assert_eq!(
            node_data.local_transformation().rotation,
            UnitQuaternion::<f32>::new(axisangle)
        )
    }

    #[test]
    fn make_pipe_section_west_dir_quarternion_applied() {
        let mut window = Window::new_hidden("test");
        let from: Coordinate = (0, 0, 0);
        let to: Coordinate = (0, 0, -1);
        let color: Color = (0.0, 1.0, 0.0);

        let node = make_pipe_section(from, to, &mut window, color);
        let node_data = node.data();

        let axisangle = Vector3::x() * PI;
        assert_eq!(
            node_data.local_transformation().rotation,
            UnitQuaternion::<f32>::new(axisangle)
        )
    }

    #[test]
    fn make_pipe_section_up_dir_quarternion_applied() {
        let mut window = Window::new_hidden("test");
        let from: Coordinate = (0, 0, 0);
        let to: Coordinate = (0, 1, 0);
        let color: Color = (0.0, 1.0, 0.0);

        let node = make_pipe_section(from, to, &mut window, color);
        let node_data = node.data();

        let axisangle = Vector3::x() * 0.0;
        assert_eq!(
            node_data.local_transformation().rotation,
            UnitQuaternion::<f32>::new(axisangle)
        )
    }

    #[test]
    fn make_pipe_section_down_dir_quarternion_applied() {
        let mut window = Window::new_hidden("test");
        let from: Coordinate = (0, 0, 0);
        let to: Coordinate = (0, -1, 0);
        let color: Color = (0.0, 1.0, 0.0);

        let node = make_pipe_section(from, to, &mut window, color);
        let node_data = node.data();

        let axisangle = Vector3::x() * PI;
        assert_eq!(
            node_data.local_transformation().rotation,
            UnitQuaternion::<f32>::new(axisangle)
        )
    }

    #[test]
    fn make_pipe_section_applies_color() {
        let mut window = Window::new_hidden("test");
        let from: Coordinate = (0, 0, 0);
        let to: Coordinate = (0, 1, 0);
        let color: Color = (0.0, 1.0, 0.0);

        let node = make_pipe_section(from, to, &mut window, color);
        let node_data = node.data();

        assert_eq!(
            node_data.get_object().data().color().to_owned(),
            Point3::<f32>::new(color.0, color.1, color.2)
        )
    }
}
