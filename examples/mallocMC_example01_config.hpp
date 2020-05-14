/*
  mallocMC: Memory Allocator for Many Core Architectures.
  https://www.hzdr.de/crp

  Copyright 2014 Institute of Radiation Physics,
                 Helmholtz-Zentrum Dresden - Rossendorf

  Author(s):  Carlchristian Eckert - c.eckert ( at ) hzdr.de

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#pragma once

// basic files for mallocMC
#include <mallocMC/mallocMC_hostclass.hpp>

// Load all available policies for mallocMC
#include <mallocMC/CreationPolicies.hpp>
#include <mallocMC/DistributionPolicies.hpp>
#include <mallocMC/OOMPolicies.hpp>
#include <mallocMC/ReservePoolPolicies.hpp>
#include <mallocMC/AlignmentPolicies.hpp>

// configurate the CreationPolicy "Scatter" to modify the default behaviour
struct ScatterHeapConfig : mallocMC::CreationPolicies::Scatter<>::HeapProperties{
  static constexpr auto pagesize = 4096;
  static constexpr auto accessblocks = 8;
  static constexpr auto regionsize = 16;
  static constexpr auto wastefactor = 2;
  static constexpr auto resetfreedpages = false;
};

struct ScatterHashConfig : mallocMC::CreationPolicies::Scatter<>::HashingProperties{
  static constexpr auto hashingK = 38183;
  static constexpr auto hashingDistMP = 17497;
  static constexpr auto hashingDistWP = 1;
  static constexpr auto hashingDistWPRel = 1;
};

// configure the DistributionPolicy "XMallocSIMD"
struct XMallocConfig : mallocMC::DistributionPolicies::XMallocSIMD<>::Properties {
  static constexpr auto pagesize = ScatterHeapConfig::pagesize;
};

// configure the AlignmentPolicy "Shrink"
struct ShrinkConfig : mallocMC::AlignmentPolicies::Shrink<>::Properties {
  static constexpr auto dataAlignment = 16;
};

// Define a new allocator and call it ScatterAllocator
// which resembles the behaviour of ScatterAlloc
using ScatterAllocator = mallocMC::Allocator<
  mallocMC::CreationPolicies::Scatter<ScatterHeapConfig, ScatterHashConfig>,
  mallocMC::DistributionPolicies::XMallocSIMD<XMallocConfig>,
  mallocMC::OOMPolicies::ReturnNull,
  mallocMC::ReservePoolPolicies::SimpleCudaMalloc,
  mallocMC::AlignmentPolicies::Shrink<ShrinkConfig>
>;