//# VERSION 3.16
// A class is only a constant by itself. In a constant collection it would be a symbol, as `#[Array]`'s is, so a
// default value with a class in a mutable collection isn't constant.
ClassElementsConst {
    const array = [Array, List];
    const nested = [1, [Array]];
    const event = (a: Array);
    const alone = Array;
    const immutable = #[Array];
    classvar class_array = [Array];
    var instance_array = [Array];
}
