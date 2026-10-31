//# VERSION 3.16
RangeArgumentPacks {
    var instance_var;

    three_arguments { ^instance_var[1, 2, 3..5] }
    four_arguments_no_end { ^instance_var[1, 2, 3, 4..] }
    dot_form { ^instance_var.[1, 2, 3..5] }
    two_arguments_are_fine { ^instance_var[1, 2..5] }
}
