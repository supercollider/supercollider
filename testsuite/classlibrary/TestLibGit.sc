TestLibGitClass {
    *initClass {
        _GitInit
        ^this.primitiveFailed;
    }
    *clone { |url, localPath|
        _GitClone
        ^this.primitiveFailed;
    }
}


TestLibGit : UnitTest {
    test_clone {
        var path = Platform.userHomeDir.asString +/+ "cloned_quarks_repo/";
        TestLibGitClass.clone(
            "https://github.com/supercollider-quarks/quarks.git",
            path
        );

        this.assert(PathName(path).entries.size != 0)
    }
}