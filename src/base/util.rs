use core::fmt;
use kiss3d::nalgebra::{UnitQuaternion, Vector3};
use rand::Rng;
use std::{collections::HashSet, slice::Iter};

pub type Coordinate = (i32, i32, i32);
pub type Color = (f32, f32, f32);

///Defines a direction in space
#[derive(Debug, Clone, Copy, PartialEq)]
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

impl fmt::Display for Direction {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let dir_str = match self {
            Direction::North => "North",
            Direction::South => "South",
            Direction::East => "East",
            Direction::West => "West",
            Direction::Up => "Up",
            Direction::Down => "Down",
        };
        write!(f, "{}", dir_str)
    }
}

impl Into<UnitQuaternion<f32>> for Direction {
    fn into(self) -> UnitQuaternion<f32> {
        match self {
            Direction::North => UnitQuaternion::new(Vector3::y() * 0.0),
            Direction::South => UnitQuaternion::new(Vector3::y() * 180.0),
            Direction::East => UnitQuaternion::new(Vector3::y() * 90.0),
            Direction::West => UnitQuaternion::new(Vector3::y() * 270.0),
            Direction::Up => UnitQuaternion::new(Vector3::x() * 90.0),
            Direction::Down => UnitQuaternion::new(Vector3::x() * 270.0),
        }
    }
}

/// Steps a coordinate in a specified direction.
pub fn step_in_dir(coord: Coordinate, dir: Direction) -> Coordinate {
    let (x, y, z) = coord;
    match dir {
        Direction::North => (x, y + 1, z),
        Direction::South => (x, y - 1, z),
        Direction::East => (x + 1, y, z),
        Direction::West => (x - 1, y, z),
        Direction::Up => (x, y, z + 1),
        Direction::Down => (x, y, z - 1),
    }
}

pub fn is_in_bounds(coord: Coordinate, bounds: Coordinate) -> bool {
    let (x, y, z) = coord;
    let (x_max, y_max, z_max) = bounds;
    x >= 0 && x < x_max && y >= 0 && y < y_max && z >= 0 && z < z_max
}

pub fn choose_random_direction(rng: &mut impl Rng) -> Direction {
    let dir = match rng.gen_range(0..6) {
        0 => Direction::North,
        1 => Direction::South,
        2 => Direction::East,
        3 => Direction::West,
        4 => Direction::Up,
        5 => Direction::Down,
        _ => panic!("Invalid direction generated"),
    };
    dir
}

/// Returns a random coordinate that is not occupied by any other pipe (including the pipe itself if
/// somehow the pipe is already on the board)
pub fn find_random_start(
    occupied_nodes: &HashSet<Coordinate>,
    bounds: Coordinate,
    rng: &mut impl Rng,
) -> Coordinate {
    //Double check if somehow there is no more space on the board
    if occupied_nodes.len() == (bounds.0 * bounds.1 * bounds.2) as usize {
        panic!("No more space on the board!");
    }

    let (x_max, y_max, z_max) = bounds;
    let mut coord = (
        rng.gen_range(0..x_max),
        rng.gen_range(0..y_max),
        rng.gen_range(0..z_max),
    );
    while occupied_nodes.contains(&coord) {
        coord = (
            rng.gen_range(0..x_max),
            rng.gen_range(0..y_max),
            rng.gen_range(0..z_max),
        );
    }
    coord
}

#[cfg(test)]
mod tests {
    use super::*;

    //could be broken down into multiple tests, but I'm lazy
    #[test]
    fn test_step_in_dir() {
        assert_eq!(step_in_dir((0, 0, 0), Direction::North), (0, 1, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::South), (0, -1, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::East), (1, 0, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::West), (-1, 0, 0));
        assert_eq!(step_in_dir((0, 0, 0), Direction::Up), (0, 0, 1));
        assert_eq!(step_in_dir((0, 0, 0), Direction::Down), (0, 0, -1));
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
}
