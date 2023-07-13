pub type Coordinate = (i32, i32, i32);

#[derive(Clone, Copy)]
pub enum Direction {
    North,
    South,
    East,
    West,
    Up,
    Down,
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
}
