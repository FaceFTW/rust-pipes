use std::ops::RangeBounds;

pub trait Rng {
    fn new() -> Self;
    fn with_seed(seed: u64) -> Self;
    fn u8(&mut self, range: impl RangeBounds<u8>) -> u8;
    fn u32(&mut self, range: impl RangeBounds<u32>) -> u32;
    fn i32(&mut self, range: impl RangeBounds<i32>) -> i32;
    fn bool(&mut self, chance: f32) -> bool;
}

struct StdRng {
    generator: fastrand::Rng,
}

impl Rng for StdRng {
    fn new() -> Self {
        todo!()
    }

    fn with_seed(seed: u64) -> Self {
        todo!()
    }

    fn u32(&mut self, range: impl RangeBounds<u32>) -> u32 {
        todo!()
    }

    fn i32(&mut self, range: impl RangeBounds<i32>) -> i32 {
        todo!()
    }

    fn u8(&mut self, range: impl RangeBounds<u8>) -> u8 {
        todo!()
    }

    fn bool(&mut self, chance: f32) -> bool {
        todo!()
    }
}
