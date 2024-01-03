#[cfg_attr(test, mockall::automock)]
pub trait EngineRng {
    // fn new() -> impl EngineRng;
    fn with_seed(seed: u64) -> Self;
    fn u8(&mut self, min: u8, max: u8) -> u8;
    fn u32(&mut self, min: u32, max: u32) -> u32;
    fn i32(&mut self, min: i32, max: i32) -> i32;
    fn bool(&mut self, chance: f64) -> bool;
    fn shuffle<T: 'static>(&mut self, slice: &mut [T]);
}

pub(crate) struct StdRng {
    generator: fastrand::Rng,
}

impl StdRng {
    pub fn new() -> Self {
        Self {
            generator: fastrand::Rng::new(),
        }
    }
}

impl EngineRng for StdRng {
    fn with_seed(seed: u64) -> Self {
        Self {
            generator: fastrand::Rng::with_seed(seed),
        }
    }

    fn u8(&mut self, min: u8, max: u8) -> u8 {
        self.generator.u8(min..max)
    }

    fn u32(&mut self, min: u32, max: u32) -> u32 {
        self.generator.u32(min..max)
    }

    fn i32(&mut self, min: i32, max: i32) -> i32 {
        self.generator.i32(min..max)
    }

    fn bool(&mut self, chance: f64) -> bool {
        self.generator.f64() < chance
    }

    fn shuffle<T>(&mut self, slice: &mut [T]) {
        self.generator.shuffle(slice);
    }
}
