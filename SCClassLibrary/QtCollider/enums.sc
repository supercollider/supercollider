QAlignment {
  classvar dict;

  *initClass {
    dict = IdentityDictionary.new;
    dict.put( \left, 16r1 | 16r80 );
    dict.put( \center, 16r4 | 16r80 );
    dict.put( \right, 16r2 | 16r80 );
    dict.put( \topLeft, 16r1 | 16r20 );
    dict.put( \topCenter, 16r4 | 16r20 );
    dict.put( \topRight, 16r2 | 16r20 );
    dict.put( \bottomLeft, 16r1 | 16r40 );
    dict.put( \bottomCenter, 16r4 | 16r40 );
    dict.put( \bottomRight, 16r2 | 16r40 );
  }

  *new { arg alignment; ^dict[alignment]; }
}

QOrientation {
  classvar dict;

  *initClass {
    dict = IdentityDictionary.new;
    dict.put( \horizontal, 1 );
    dict.put( \vertical, 2 );
  }

  *new { arg alignment; ^dict[alignment]; }
}
