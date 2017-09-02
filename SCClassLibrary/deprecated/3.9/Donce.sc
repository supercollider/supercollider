Donce : DUGen {
    *new { arg in;
        this.deprecated(thisMethod);
        ^this.multiNew('demand', in)
    }
}
