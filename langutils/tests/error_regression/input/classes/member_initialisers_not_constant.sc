//# VERSION 3.16
NotConstant {
    const fine = 1;
    classvar changes = 2;
    var refers_to_const = fine;
    classvar class_refers_to_const = fine;
    var refers_to_classvar = changes;
    var message = 1.foo;
    var environment = ~env;
    var reference = `1;
    var unknown_name = other;
    var curry = _;

    immutable_array { ^#[1, 2.foo, ~env] }
}
