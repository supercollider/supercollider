#!/bin/bash
make scsynth
cp server/scsynth/*.js ../wasm/
cp server/scsynth/*.wasm ../wasm/
