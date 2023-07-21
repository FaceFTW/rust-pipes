use core::fmt;

#[derive(Debug, Copy, Clone)]
pub struct NoDirectionError;

impl fmt::Display for NoDirectionError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "No Valid Direction Found")
    }
}
