
TestIdentityDictionary : UnitTest {
    test_put {
        this.assertEquals(
            ().put(\meow, 1),
            (meow: 1),
            "Normal put works"
        );

        this.assertEquals(
            ().put(key: \meow, value: 1),
            (meow: 1),
            "Put with keyword arguments work"
        );

        this.assertEquals(
            ().put(\meow, 1, 10),
            (meow: 1),
            "Normal put works with extra arguments"
        );

        this.assertEquals(
            ().put(key: \meow, value: 1, woof: 10),
            (meow: 1),
            "Put with extra keyword arguments works"
        );

        this.assertEquals(
            ().put(\meow, 1, woof: 10),
            (meow: 1),
            "Put with extra keyword arguments and normal arguments work"
        );

    }
}
