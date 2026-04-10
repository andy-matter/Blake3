# BLAKE3

BLAKE3 is a cryptographic hash function that is:

- **Much faster** than MD5, SHA-1, SHA-2, SHA-3, and BLAKE2.
- **Secure**, unlike MD5 and SHA-1. And secure against length extension,
  unlike SHA-2.
- **Highly parallelizable** across any number of threads and SIMD lanes,
  because it's a Merkle tree on the inside.
- Capable of **verified streaming** and **incremental updates**, again
  because it's a Merkle tree.
- A **PRF**, **MAC**, **KDF**, and **XOF**, as well as a regular hash.
- **One algorithm with no variants**, which is fast on x86-64 and also
  on smaller architectures.

The [chart below](https://github.com/BLAKE3-team/BLAKE3-specs/blob/master/benchmarks/bar_chart.py)
is an example benchmark of 16 KiB inputs on a Cascade Lake-SP 8275CL server CPU
from 2019. For more detailed benchmarks, see the
[BLAKE3 paper](https://github.com/BLAKE3-team/BLAKE3-specs/blob/master/blake3.pdf).

<p align="center">
<img src="media/speed.svg" alt="performance graph">
</p>

BLAKE3 is based on an optimized instance of the established hash
function [BLAKE2](https://blake2.net) and on the [original Bao tree
mode](https://github.com/oconnor663/bao/blob/master/docs/spec_0.9.1.md).
The specifications and design rationale are available in the [BLAKE3
paper](https://github.com/BLAKE3-team/BLAKE3-specs/blob/master/blake3.pdf).
The default output size is 256 bits. The current version of
[Bao](https://github.com/oconnor663/bao) implements verified streaming
with BLAKE3.

This repository is the official implementation of BLAKE3. It includes:

* The [`blake3`](https://crates.io/crates/blake3) Rust crate, which
  includes optimized implementations for SSE2, SSE4.1, AVX2, AVX-512,
  NEON, and WASM, with automatic runtime CPU feature detection on x86.
  The `rayon` feature provides multithreading.

* The [`b3sum`](https://crates.io/crates/b3sum) Rust crate, which
  provides a command line interface. It uses multithreading by default,
  making it an order of magnitude faster than e.g. `sha256sum` on
  typical desktop hardware.

* The [C implementation](c), which like the Rust implementation includes SIMD
  optimizations (all except WASM), CPU feature detection on x86, and optional
  multithreading. See [`c/README.md`](c/README.md).

* The [Rust reference implementation](reference_impl/reference_impl.rs),
  which is discussed in Section 5.1 of the [BLAKE3
  paper](https://github.com/BLAKE3-team/BLAKE3-specs/blob/master/blake3.pdf).
  This implementation is much smaller and simpler than the optimized
  ones above. If you want to see how BLAKE3 works, or you're writing a
  port that doesn't need multithreading or SIMD optimizations, start
  here. Ports of the reference implementation to other languages are
  hosted in separate repositories
  ([C](https://github.com/oconnor663/blake3_reference_impl_c),
  [Python](https://github.com/oconnor663/pure_python_blake3)).

* A [set of test
  vectors](https://github.com/BLAKE3-team/BLAKE3/blob/master/test_vectors/test_vectors.json)
  that covers extended outputs, all three modes, and a variety of input
  lengths.

* [![Actions Status](https://github.com/BLAKE3-team/BLAKE3/workflows/tests/badge.svg)](https://github.com/BLAKE3-team/BLAKE3/actions)

BLAKE3 was designed by:

* [@oconnor663] (Jack O'Connor)
* [@sneves] (Samuel Neves)
* [@veorq] (Jean-Philippe Aumasson)
* [@zookozcash] (Zooko)

The development of BLAKE3 was sponsored by [Electric Coin Company](https://electriccoin.co).

BLAKE3 is also [specified](https://c2sp.org/BLAKE3) in the [Community
Cryptography Specification Project (C2SP)](https://c2sp.org).

*NOTE: BLAKE3 is not a password hashing algorithm, because it's
designed to be fast, whereas password hashing should not be fast. If you
hash passwords to store the hashes or if you derive keys from passwords,
we recommend [Argon2](https://github.com/P-H-C/phc-winner-argon2).*

## Licenses

This work is released into the public domain with [CC0 1.0](./LICENSE_CC0).
Alternatively, it is licensed under any of the following:

* [Apache 2.0](./LICENSE_A2)
* [Apache 2.0 with LLVM exceptions](./LICENSE_A2LLVM)

## Miscellany

- [@veorq] and [@oconnor663] did [an interview with Cryptography FM](https://cryptography.fireside.fm/3).
- [@oconnor663] did [an interview with Saito](https://www.youtube.com/watch?v=cJkmIt7yN_E).

[@oconnor663]: https://github.com/oconnor663
[@sneves]: https://github.com/sneves
[@veorq]: https://github.com/veorq
[@zookozcash]: https://github.com/zookozcash
