// Some JITLib deprecations.

+ Monitor {
    *warnPlayN {
        this.deprecated(thisMethod);
    }
    *warnPlayN_ { |flag|
        this.deprecated(thisMethod);
    }
}

+ NodeProxy {
    server_ { | inServer |
        this.deprecated(thisMethod);
        if(this.isNeutral.not) {
            this.end;
            loaded = false;
        };
        server = inServer;
    }

    mapn { | ... args |
        this.deprecated(thisMethod, NodeProxy.findMethod(\map));
        this.map(*args)
    }

    xmapn { | ... args |
        this.deprecated(thisMethod, NodeProxy.findMethod(\xmap));
        this.xFadePerform(\map, args)
    }
}

+ NodeMap {
    setn {
        ^this.deprecated(thisMethod, this.class.findRespondingMethodFor(\set))
    }

    mapn {
        ^this.deprecated(thisMethod, this.class.findRespondingMethodFor(\set))
    }

    map {
        ^this.deprecated(thisMethod, this.class.findRespondingMethodFor(\set))
    }
}

+ ProxyNodeMap {
    isMapped { |key|
        var val = this.at(key);
        this.deprecated(thisMethod);
        ^val.isNumber.not and: { val.isSequenceableCollection.not }
    }
}
