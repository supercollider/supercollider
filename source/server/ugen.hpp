//  ugen
//  Copyright (C) 2008 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#ifndef SERVER_UGEN_HPP
#define SERVER_UGEN_HPP

#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>

#include "dsp_context.hpp"
#include "node_types.hpp"
#include "sample_types.hpp"


namespace nova
{

class ugen;

enum ugen_arg_type
{
    signal,
    control,
    event,
    constant,
    none
};

typedef std::vector<const_aligned_sample_ptr> inblock_array_t;
typedef std::vector<aligned_sample_ptr> outblock_array_t;

typedef boost::uint32_t uint32;


/**
 *  \todo for now we use per-ugen/per-synth sample blocks.
 *  in order to reduce the memory usage and improve cache locality, the use of
 *  thread-specific buffers for each synthdef should be considered ...
 *  but how to formulate it? (thread-id as call argument?)
 *
 *  */
class ugen
{
public:
    virtual ~ugen (void) {}

    /** set the used audio buffers */
    virtual void prepare(inblock_array_t const & inblocks, outblock_array_t const & outblocks) = 0;

    /** initialize the ugen */
    virtual void prepare(const dsp_context *) {};
};


/** wrapper class for running ugens
 *
 *  holds ownership of the ugen class
 *  */
class ugen_runner
{
public:
    typedef void (*runfn)(ugen *, uint32, const dsp_context*);

private:
    /* wrapper to call Ugen member functions */
    template <typename Ugen>
    struct default_runner_function
    {
        static void run(ugen * obj, uint32 samples, const dsp_context* c)
        {
            static_cast<Ugen*>(obj)->run(samples, c);
        }

        static void run_unrolled(ugen * obj, uint32 samples, const dsp_context* c)
        {
            static_cast<Ugen*>(obj)->run_unrolled(samples, c);
        }
    };

public:
    ugen_runner(ugen * obj, runfn run_):
        obj(obj), run_(run_), run_unrolled_(run_)
    {}

    ugen_runner(ugen * obj, runfn run_, runfn run_unrolled_):
        obj(obj), run_(run_), run_unrolled_(run_unrolled_)
    {}

    template <typename Ugen>
    explicit ugen_runner(Ugen * obj):
        obj(obj),
        run_(default_runner_function<Ugen>::run),
        run_unrolled_(default_runner_function<Ugen>::run)
    {}

#if 0
    /* copy constructor, has move semantics for the ugen class */
    ugen_runner(ugen_runner & rhs):
        obj(rhs.obj.release()), run_(rhs.run_), run_unrolled_(rhs.run_unrolled_)
    {}
#endif

    template <typename Ugen>
    void set_unrolled(void)
    {
        run_unrolled_ = default_runner_function<Ugen>::run;
    }

    /* copy constructor should be fine */
    void run(uint32 samples, const dsp_context * c)
    {
        run_(obj.get(), samples, c);
    }

    void run_unrolled(uint32 samples, const dsp_context * c)
    {
        run_unrolled_(obj.get(), samples, c);
    }

    void prepare(const dsp_context * context)
    {
        obj->prepare(context);
    }

private:
    shared_ptr<ugen> obj;       /**< \todo later implement move semantics with auto_ptr or unique_ptr */
    runfn run_;
    runfn run_unrolled_;
};

class ugen_chain
{
public:
    void prepare(const dsp_context * context)
    {
        for(std::size_t i = 0; i != chain.size(); ++i)
            chain[i].prepare(context);
    }

    void run(uint32 samples, const dsp_context * context)
    {
#if 0
        assert(!chain.empty());

        chain[0].run(samples, context);
        for(std::size_t i = 1; i != chain.size(); ++i)
            chain[i].run(samples, context);

#else
        for(std::size_t i = 0; i != chain.size(); ++i)
            chain[i].run(samples, context);
#endif
    }

    void run_unrolled(uint32 samples, const dsp_context * context)
    {
#if 0
        assert(!chain.empty());

        chain[0].run_unrolled(samples, context);
        for(std::size_t i = 1; i != chain.size(); ++i)
            chain[i].run_unrolled(samples, context);

#else
        for(std::size_t i = 0; i != chain.size(); ++i)
            chain[i].run_unrolled(samples, context);
#endif
    }

private:
    std::vector<ugen_runner, server_node_allocator<ugen_runner> > chain;
};

typedef std::vector<ugen_arg_type> ugen_arg_signature;

class ugen_factory:
    public boost::noncopyable
{
public:
    virtual ~ugen_factory(void)
    {}

    virtual ugen_runner create_instance (ugen_arg_signature const & sig, uint32 samples) = 0;
};


} /* namespace nova */

#endif /* SERVER_UGEN_HPP */
