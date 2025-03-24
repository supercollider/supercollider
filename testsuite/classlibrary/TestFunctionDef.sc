TestFunctionDef : UnitTest {
    test_filename {
        var path = thisFunctionDef.filename;
        path.debug(\path);
        this.assertEquals(path.basename, "TestFunctionDef.sc", "thisFunctionDef should return the current file");
    }

    test_filename_different_file {
        var path = (thisFunctionDef.filename.dirname +/+ "TestFunctionDef_filename.scd").load;
        this.assertEquals(path.basename, "TestFunctionDef_filename.scd", "thisFunctionDef should return the other file when loaded");
    }

    test_filename_different_file_routine {
        var cond = CondVar();
        var done = false;
        fork {
            var path = (thisFunctionDef.filename.dirname +/+ "TestFunctionDef_filename.scd").load;
            this.assertEquals(path.basename, "TestFunctionDef_filename.scd", "thisFunctionDef should return the other file when loaded");
            done = true;
            cond.signalOne;
        };

        cond.wait { done };
    }
}
