use super::rng::EngineRng;
use cfg_if::cfg_if;
use std::{collections::HashSet, slice::Iter, time::Duration};
use three_d::{Deg, Instances, Mat4, Srgba, Vec3};

pub type Coordinate = (i32, i32, i32);
pub type Color = (u8, u8, u8);

const PIPE_RADIUS: f32 = 0.15;
const BALL_JOINT_RADIUS: f32 = 0.3;

//=============================================
// State-related Utilities
//=============================================
///Defines a direction in space
#[derive(Debug, Copy, Clone, PartialEq)]
pub enum Direction {
    North,
    South,
    East,
    West,
    Up,
    Down,
}

impl Direction {
    pub fn iterator() -> Iter<'static, Direction> {
        [
            Direction::North,
            Direction::South,
            Direction::East,
            Direction::West,
            Direction::Up,
            Direction::Down,
        ]
        .iter()
    }
}

impl From<Direction> for Mat4 {
    fn from(val: Direction) -> Self {
        match val {
            Direction::North => Mat4::from_angle_y(Deg(0.0)),
            Direction::South => Mat4::from_angle_y(Deg(180.0)),
            Direction::East => Mat4::from_angle_y(Deg(-90.0)),
            Direction::West => Mat4::from_angle_y(Deg(90.0)),
            Direction::Up => Mat4::from_angle_z(Deg(90.0)),
            Direction::Down => Mat4::from_angle_z(Deg(270.0)),
        }
    }
}

impl From<Coordinate> for Direction {
    fn from(value: Coordinate) -> Self {
        let (x, y, z) = value;
        match (x, y, z) {
            (x, 0, 0) if x > 0 => Direction::North,
            (x, 0, 0) if x < 0 => Direction::South,
            (0, y, 0) if y > 0 => Direction::Up,
            (0, y, 0) if y < 0 => Direction::Down,
            (0, 0, z) if z > 0 => Direction::East,
            (0, 0, z) if z < 0 => Direction::West,
            _ => panic!(
                "Could not convert coordinate/distance to a cardinal direction, {:?}",
                value
            ),
        }
    }
}

/// Steps a coordinate in a specified direction.
pub fn step_in_dir(coord: Coordinate, dir: Direction) -> Coordinate {
    let (x, y, z) = coord;
    match dir {
        Direction::North => (x + 1, y, z),
        Direction::South => (x - 1, y, z),
        Direction::East => (x, y, z + 1),
        Direction::West => (x, y, z - 1),
        Direction::Up => (x, y + 1, z),
        Direction::Down => (x, y - 1, z),
    }
}

pub fn is_in_bounds(coord: Coordinate, bounds: Coordinate) -> bool {
    let (x, y, z) = coord;
    let (x_max, y_max, z_max) = bounds;
    x >= 0 && x < x_max && y >= 0 && y < y_max && z >= 0 && z < z_max
}

pub fn choose_random_direction(rng: &mut impl EngineRng) -> Direction {
    match rng.u8(0, 6) {
        0 => Direction::North,
        1 => Direction::South,
        2 => Direction::East,
        3 => Direction::West,
        4 => Direction::Up,
        5 => Direction::Down,
        _ => panic!("Invalid direction generated"),
    }
}

/// Returns a random coordinate that is not occupied by any other pipe (including the pipe itself if
/// somehow the pipe is already on the board)
pub fn find_random_start(
    occupied_nodes: &HashSet<Coordinate>,
    bounds: Coordinate,
    rng: &mut impl EngineRng,
) -> Coordinate {
    //Double check if somehow there is no more space on the board
    if occupied_nodes.len() >= ((bounds.0 + 1) * (bounds.1 + 1) * (bounds.2 + 1)) as usize {
        panic!("No more space on the board!");
    }

    let (x_max, y_max, z_max) = bounds;
    let mut coord = (rng.i32(0, x_max), rng.i32(0, y_max), rng.i32(0, z_max));
    while occupied_nodes.contains(&coord) {
        coord = (rng.i32(0, x_max), rng.i32(0, y_max), rng.i32(0, z_max));
    }
    coord
}

fn color_to_srgba(color: Color) -> Srgba {
    Srgba {
        r: color.0,
        g: color.1,
        b: color.2,
        a: 255,
    }
}

//=============================================
// Rendering Related Utilities
//=============================================
///Creates an instance of a pipe segment with the required transformations.
pub fn make_instanced_pipe_section(
    instance_struct: &mut Instances,
    from: Coordinate,
    to: Coordinate,
    color: Color,
) {
    let (from_x, from_y, from_z) = from;
    let (to_x, to_y, to_z) = to;
    let delta = (to_x - from_x, to_y - from_y, to_z - from_z);
    let pipe_len = ((delta.0.pow(2) + delta.1.pow(2) + delta.2.pow(2)) as f32).sqrt();

    let translation_matrix =
        Mat4::from_translation(Vec3::new(from_x as f32, from_y as f32, from_z as f32));
    let rotation_matrix: Mat4 = Direction::from(delta).into();
    let scale_matrix = Mat4::from_nonuniform_scale(pipe_len, PIPE_RADIUS, PIPE_RADIUS);
    let transform = translation_matrix * rotation_matrix * scale_matrix;

    instance_struct.transformations.push(transform);
    instance_struct
        .colors
        .as_mut()
        .expect("Instance Struct should have Color Instance Vector!")
        .push(color_to_srgba(color));
}

pub fn make_instanced_ball_joint(instance_struct: &mut Instances, pos: Coordinate, color: Color) {
    let (pos_x, pos_y, pos_z) = pos;

    let translation_matrix =
        Mat4::from_translation(Vec3::new(pos_x as f32, pos_y as f32, pos_z as f32));
    let scale_matrix = Mat4::from_scale(BALL_JOINT_RADIUS); //Scale is uniform here unlike pipes
    let transform = translation_matrix * scale_matrix;

    instance_struct.transformations.push(transform);
    instance_struct
        .colors
        .as_mut()
        .expect("Instance Struct should have Color Instances Vector!")
        .push(color_to_srgba(color));
}

//============================================
// Time-related utilities
//============================================

cfg_if! {
    if #[cfg(not(target_arch="wasm32"))]{
        ///The following code is to create a "Shim" struct to allow for
        /// instants to be used on cross-platforms that don't
        /// have std::time::Instant implementations (i.e. WASM). This version
        /// uses the implementation that exists for standard platforms
        /// such as x86 or ARM.
        ///
        /// Code is a reduced form of the following comment
        /// https://github.com/rust-lang/rust/issues/48564#issuecomment-698712971
        #[derive(Clone, Copy, Debug, PartialEq, Eq, PartialOrd, Ord, Hash)]
        pub struct InstantShim(std::time::Instant);

        impl InstantShim {
            pub fn now() -> Self { Self(std::time::Instant::now()) }
            pub fn elapsed(&self) -> Duration { self.0.elapsed() }
        }
    } else {
        use wasm_bindgen::prelude::*;
        #[wasm_bindgen(inline_js = r#"export function performance_now() {return performance.now();}"#)]
        extern "C" {
            fn performance_now() -> f64;
        }

        ///The following code is to create a "Shim" struct to allow for
        /// instants to be used on cross-platforms that don't
        /// have std::time::Instant implementations (i.e. WASM).
        /// This version uses the performance.now() method in JS
        /// engines to get the time necessary.
        ///
        /// Code is a reduced form of the following comment
        /// https://github.com/rust-lang/rust/issues/48564#issuecomment-698712971
        #[derive(Clone, Copy, Debug, PartialEq, Eq, PartialOrd, Ord, Hash)]
        pub struct InstantShim(u64);

        impl InstantShim {
            pub fn now() -> Self { Self((performance_now() * 1000.0) as u64) }
            pub fn duration_since(&self, earlier: InstantShim) -> Duration { Duration::from_micros(self.0 - earlier.0) }
            pub fn elapsed(&self) -> Duration { Self::now().duration_since(*self) }
        }
    }
}

//=============================================
// Unit Tests
//=============================================
#[cfg(test)]
mod tests {
    use super::*;
    use super::{color_to_srgba, PIPE_RADIUS};
    use crate::engine::rng::MockEngineRng;
    use mockall::Sequence;
    use three_d::{Instances, Mat4, Vec3};

    #[test]
    pub fn make_instanced_pipe_section_creates_expected_instances() {
        let mut pipe_instances = Instances {
            transformations: Vec::new(),
            colors: Some(Vec::new()),
            ..Default::default()
        };

        let scale_mat = Mat4::from_nonuniform_scale(1.0, PIPE_RADIUS, PIPE_RADIUS);

        let rot_mat_north: Mat4 = Direction::North.into();
        let rot_mat_south: Mat4 = Direction::South.into();
        let rot_mat_east: Mat4 = Direction::East.into();
        let rot_mat_west: Mat4 = Direction::West.into();
        let rot_mat_up: Mat4 = Direction::Up.into();
        let rot_mat_down: Mat4 = Direction::Down.into();

        let expected_transforms = vec![
            Mat4::from_translation(Vec3::new(0.0, 0.0, 0.0)) * rot_mat_north * scale_mat,
            Mat4::from_translation(Vec3::new(1.0, 0.0, 0.0)) * rot_mat_south * scale_mat,
            Mat4::from_translation(Vec3::new(0.0, 1.0, 0.0)) * rot_mat_east * scale_mat,
            Mat4::from_translation(Vec3::new(0.0, 0.0, 1.0)) * rot_mat_west * scale_mat,
            Mat4::from_translation(Vec3::new(1.0, 1.0, 0.0)) * rot_mat_up * scale_mat,
            Mat4::from_translation(Vec3::new(1.0, 0.0, 1.0)) * rot_mat_down * scale_mat,
        ];

        let expected_colors = vec![
            color_to_srgba((0, 0, 0)),
            color_to_srgba((255, 0, 0)),
            color_to_srgba((0, 255, 0)),
            color_to_srgba((0, 0, 255)),
            color_to_srgba((255, 255, 0)),
            color_to_srgba((255, 255, 255)),
        ];

        make_instanced_pipe_section(&mut pipe_instances, (0, 0, 0), (1, 0, 0), (0, 0, 0));
        make_instanced_pipe_section(&mut pipe_instances, (1, 0, 0), (0, 0, 0), (255, 0, 0));
        make_instanced_pipe_section(&mut pipe_instances, (0, 1, 0), (0, 1, 1), (0, 255, 0));
        make_instanced_pipe_section(&mut pipe_instances, (0, 0, 1), (0, 0, 0), (0, 0, 255));
        make_instanced_pipe_section(&mut pipe_instances, (1, 1, 0), (1, 2, 0), (255, 255, 0));
        make_instanced_pipe_section(&mut pipe_instances, (1, 0, 1), (1, -1, 1), (255, 255, 255));

        assert_eq!(
            pipe_instances.transformations.len(),
            expected_transforms.len()
        );

        assert!(pipe_instances.colors.is_some());
        let actual_colors = pipe_instances
            .colors
            .expect("Colors Vector was not found in Instance Struct!");

        assert_eq!(actual_colors.len(), expected_colors.len());

        for i in 0..pipe_instances.transformations.len() {
            let expected_transform = expected_transforms[i];
            let actual_transform = pipe_instances.transformations[i];
            assert_eq!(expected_transform, actual_transform);

            let expected_color = expected_colors[i];
            let actual_color = actual_colors[i];
            assert_eq!(expected_color, actual_color);
        }
    }

    #[test]
    pub fn make_instanced_ball_joint_creates_expected_instances() {
        let mut ball_instances = Instances {
            transformations: Vec::new(),
            colors: Some(Vec::new()),
            ..Default::default()
        };

        let scale_mat = Mat4::from_scale(BALL_JOINT_RADIUS);

        let expected_transforms = vec![
            Mat4::from_translation(Vec3::new(0.0, 0.0, 0.0)) * scale_mat,
            Mat4::from_translation(Vec3::new(1.0, 0.0, 0.0)) * scale_mat,
            Mat4::from_translation(Vec3::new(0.0, 1.0, 0.0)) * scale_mat,
            Mat4::from_translation(Vec3::new(0.0, 0.0, 1.0)) * scale_mat,
        ];

        let expected_colors = vec![
            color_to_srgba((0, 0, 0)),
            color_to_srgba((255, 0, 0)),
            color_to_srgba((0, 255, 0)),
            color_to_srgba((0, 0, 255)),
        ];

        make_instanced_ball_joint(&mut ball_instances, (0, 0, 0), (0, 0, 0));
        make_instanced_ball_joint(&mut ball_instances, (1, 0, 0), (255, 0, 0));
        make_instanced_ball_joint(&mut ball_instances, (0, 1, 0), (0, 255, 0));
        make_instanced_ball_joint(&mut ball_instances, (0, 0, 1), (0, 0, 255));

        assert_eq!(
            ball_instances.transformations.len(),
            expected_transforms.len()
        );

        assert!(ball_instances.colors.is_some());
        let actual_colors = ball_instances
            .colors
            .expect("Colors Vector was not found in Instance Struct!");

        assert_eq!(actual_colors.len(), expected_colors.len());

        for i in 0..ball_instances.transformations.len() {
            let expected_transform = expected_transforms[i];
            let actual_transform = ball_instances.transformations[i];
            assert_eq!(expected_transform, actual_transform);

            let expected_color = expected_colors[i];
            let actual_color = actual_colors[i];
            assert_eq!(expected_color, actual_color);
        }
    }

    #[test]
    fn test_step_in_dir() {
        assert_eq!(step_in_dir((0, 0, 0), Direction::North), (1, 0, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::South), (-1, 0, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::East), (0, 0, 1));
        assert_eq!(step_in_dir((0, 0, 0), Direction::West), (0, 0, -1));
        assert_eq!(step_in_dir((0, 0, 0), Direction::Up), (0, 1, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::Down), (0, -1, 0));
    }

    #[test]
    fn test_dir_iterator() {
        let mut dir_iter = Direction::iterator();
        assert_eq!(dir_iter.next(), Some(&Direction::North));
        assert_eq!(dir_iter.next(), Some(&Direction::South));
        assert_eq!(dir_iter.next(), Some(&Direction::East));
        assert_eq!(dir_iter.next(), Some(&Direction::West));
        assert_eq!(dir_iter.next(), Some(&Direction::Up));
        assert_eq!(dir_iter.next(), Some(&Direction::Down));
        assert_eq!(dir_iter.next(), None);
    }

    #[test]
    fn test_direction_from_coordinate_cardinal_directions() {
        assert_eq!(Direction::from((1, 0, 0)), Direction::North);
        assert_eq!(Direction::from((-1, 0, 0)), Direction::South);
        assert_eq!(Direction::from((0, 1, 0)), Direction::Up);
        assert_eq!(Direction::from((0, -1, 0)), Direction::Down);
        assert_eq!(Direction::from((0, 0, 1)), Direction::East);
        assert_eq!(Direction::from((0, 0, -1)), Direction::West);
    }

    #[test]
    #[should_panic]
    fn test_direction_from_coordinate_vector_dirs_panic() {
        let _test2 = Direction::from((1, 2, 3));
    }

    #[test]
    fn choose_random_direction_returns_based_on_rng() {
        let mut mock_rng = MockEngineRng::new();
        let mut seq = Sequence::new();

        mock_rng
            .expect_u8()
            .once()
            .in_sequence(&mut seq)
            .return_const(0);
        mock_rng
            .expect_u8()
            .once()
            .in_sequence(&mut seq)
            .return_const(1);
        mock_rng
            .expect_u8()
            .once()
            .in_sequence(&mut seq)
            .return_const(2);
        mock_rng
            .expect_u8()
            .once()
            .in_sequence(&mut seq)
            .return_const(3);
        mock_rng
            .expect_u8()
            .once()
            .in_sequence(&mut seq)
            .return_const(4);
        mock_rng
            .expect_u8()
            .once()
            .in_sequence(&mut seq)
            .return_const(5);

        assert_eq!(choose_random_direction(&mut mock_rng), Direction::North);
        assert_eq!(choose_random_direction(&mut mock_rng), Direction::South);
        assert_eq!(choose_random_direction(&mut mock_rng), Direction::East);
        assert_eq!(choose_random_direction(&mut mock_rng), Direction::West);
        assert_eq!(choose_random_direction(&mut mock_rng), Direction::Up);
        assert_eq!(choose_random_direction(&mut mock_rng), Direction::Down);
    }

    #[test]
    #[should_panic]
    fn test_direction_from_coordinate_zero_vector_panics() {
        let _test1 = Direction::from((0, 0, 0));
    }

    #[test]
    #[should_panic]
    fn test_find_random_start_no_more_nodes_exact_len_panics() {
        let mut mock_rng = MockEngineRng::new();
        let mut occupied: HashSet<Coordinate> = HashSet::new();
        for x in 0..2 {
            for y in 0..2 {
                for z in 0..2 {
                    occupied.insert((x, y, z) as Coordinate);
                }
            }
        }

        find_random_start(&occupied, (2, 2, 2), &mut mock_rng);
    }

    #[test]
    #[should_panic]
    fn test_find_random_start_no_more_nodes_extra_len_panics() {
        let mut mock_rng = MockEngineRng::new();
        let mut occupied: HashSet<Coordinate> = HashSet::new();
        for x in 0..2 {
            for y in 0..2 {
                for z in 0..2 {
                    occupied.insert((x, y, z) as Coordinate);
                }
            }
        }
        occupied.insert((3, 2, 2));

        find_random_start(&occupied, (2, 2, 2), &mut mock_rng);
    }

    #[test]
    fn test_find_random_start_no_occupied_uses_rng() {
        let mut mock_rng = MockEngineRng::new();
        let occupied: HashSet<Coordinate> = HashSet::new();

        mock_rng.expect_i32().times(3).return_const(1);

        let result = find_random_start(&occupied, (2, 2, 2), &mut mock_rng);
        assert_eq!(result, (1, 1, 1))
    }

    #[test]
    fn test_find_random_start_some_occupied_did_collision_loops_to_next_val() {
        let mut mock_rng = MockEngineRng::new();
        let mut occupied: HashSet<Coordinate> = HashSet::new();
        occupied.insert((1, 1, 1));
        let mut seq = Sequence::new();

        mock_rng
            .expect_i32()
            .times(3)
            .return_const(1)
            .in_sequence(&mut seq);

        mock_rng
            .expect_i32()
            .times(3)
            .return_const(2)
            .in_sequence(&mut seq);

        let result = find_random_start(&occupied, (2, 2, 2), &mut mock_rng);
        assert_eq!(result, (2, 2, 2))
    }

    #[test]
    fn test_find_random_start_some_occupied_no_collision_does_not_loop() {
        let mut mock_rng = MockEngineRng::new();
        let mut occupied: HashSet<Coordinate> = HashSet::new();
        occupied.insert((0, 0, 0));

        mock_rng.expect_i32().times(3).return_const(1);

        let result = find_random_start(&occupied, (2, 2, 2), &mut mock_rng);
        assert_eq!(result, (1, 1, 1))
    }

    #[test]
    fn test_is_in_bounds() {
        let bounds = (2, 2, 2);
        assert!(!is_in_bounds((-1, 0, 0), bounds));
        assert!(is_in_bounds((0, 0, 0), bounds));
        assert!(is_in_bounds((1, 0, 0), bounds));
        assert!(!is_in_bounds((2, 0, 0), bounds));

        assert!(!is_in_bounds((0, -1, 0), bounds));
        assert!(is_in_bounds((0, 0, 0), bounds));
        assert!(is_in_bounds((0, 1, 0), bounds));
        assert!(!is_in_bounds((0, 2, 0), bounds));

        assert!(!is_in_bounds((0, 0, -1), bounds));
        assert!(is_in_bounds((0, 0, 0), bounds));
        assert!(is_in_bounds((0, 0, 1), bounds));
        assert!(!is_in_bounds((0, 0, 2), bounds));
    }


}
