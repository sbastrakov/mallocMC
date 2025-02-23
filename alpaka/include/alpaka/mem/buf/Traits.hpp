/* Copyright 2022 Alexander Matthes, Benjamin Worpitz, Andrea Bocci, Bernhard Manfred Gruber
 *
 * This file is part of alpaka.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <alpaka/core/Common.hpp>
#include <alpaka/mem/view/Traits.hpp>

namespace alpaka
{
    //! The CPU device handle.
    class DevCpu;

    //! The buffer traits.
    namespace trait
    {
        //! The memory buffer type trait.
        template<typename TDev, typename TElem, typename TDim, typename TIdx, typename TSfinae = void>
        struct BufType;

        //! The memory allocator trait.
        template<typename TElem, typename TDim, typename TIdx, typename TDev, typename TSfinae = void>
        struct BufAlloc;

        //! The stream-ordered memory allocator trait.
        template<typename TElem, typename TDim, typename TIdx, typename TDev, typename TSfinae = void>
        struct AsyncBufAlloc;

        //! The stream-ordered memory allocator capability trait.
        template<typename TDim, typename TDev>
        struct HasAsyncBufSupport : public std::false_type
        {
        };

        //! The pinned/mapped memory allocator trait.
        template<typename TElem, typename TDim, typename TIdx, typename TDev>
        struct BufAllocMapped;

        //! The memory mapping trait.
        template<typename TBuf, typename TDev, typename TSfinae = void>
        struct Map;

        //! The memory unmapping trait.
        template<typename TBuf, typename TDev, typename TSfinae = void>
        struct Unmap;

        //! The memory pinning trait.
        template<typename TBuf, typename TSfinae = void>
        struct Pin;

        //! The memory unpinning trait.
        template<typename TBuf, typename TSfinae = void>
        struct Unpin;

        //! The memory pin state trait.
        template<typename TBuf, typename TSfinae = void>
        struct IsPinned;

        //! The memory prepareForAsyncCopy trait.
        template<typename TBuf, typename TSfinae = void>
        struct PrepareForAsyncCopy;
    } // namespace trait

    //! The memory buffer type trait alias template to remove the ::type.
    template<typename TDev, typename TElem, typename TDim, typename TIdx>
    using Buf = typename trait::BufType<alpaka::Dev<TDev>, TElem, TDim, TIdx>::type;

    //! Allocates memory on the given device.
    //!
    //! \tparam TElem The element type of the returned buffer.
    //! \tparam TIdx The linear index type of the buffer.
    //! \tparam TExtent The extent type of the buffer.
    //! \tparam TDev The type of device the buffer is allocated on.
    //! \param dev The device to allocate the buffer on.
    //! \param extent The extent of the buffer.
    //! \return The newly allocated buffer.
    template<typename TElem, typename TIdx, typename TExtent, typename TDev>
    ALPAKA_FN_HOST auto allocBuf(TDev const& dev, TExtent const& extent = TExtent())
    {
        return trait::BufAlloc<TElem, Dim<TExtent>, TIdx, TDev>::allocBuf(dev, extent);
    }

    //! Allocates stream-ordered memory on the given device.
    //!
    //! \tparam TElem The element type of the returned buffer.
    //! \tparam TIdx The linear index type of the buffer.
    //! \tparam TExtent The extent type of the buffer.
    //! \tparam TQueue The type of queue used to order the buffer allocation.
    //! \param queue The queue used to order the buffer allocation.
    //! \param extent The extent of the buffer.
    //! \return The newly allocated buffer.
    template<typename TElem, typename TIdx, typename TExtent, typename TQueue>
    ALPAKA_FN_HOST auto allocAsyncBuf(TQueue queue, TExtent const& extent = TExtent())
    {
        return trait::AsyncBufAlloc<TElem, Dim<TExtent>, TIdx, alpaka::Dev<TQueue>>::allocAsyncBuf(queue, extent);
    }

    //! Check if the given device can allocate a stream-ordered memory buffer of the given dimensionality.
    //!
    //! TDev is the type of device to allocate the buffer on.
    //! TDim is the dimensionality of the buffer to allocate.
    /* TODO: Remove the following pragmas once support for clang 5 and 6 is removed. They are necessary because these
    /  clang versions incorrectly warn about a missing 'extern'. */
#if BOOST_COMP_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif
    template<typename TDev, typename TDim>
    constexpr inline bool hasAsyncBufSupport = trait::HasAsyncBufSupport<TDim, TDev>::value;
#if BOOST_COMP_CLANG
#    pragma clang diagnostic pop
#endif

    //! Allocates pinned/mapped memory on host, accessible by the given device.
    //!
    //! \tparam TElem The element type of the returned buffer.
    //! \tparam TIdx The linear index type of the buffer.
    //! \tparam TExtent The extent type of the buffer.
    //! \tparam TDev The type of device the buffer is accessible from.
    //! \param host The host device to allocate the buffer on.
    //! \param dev The device to make the memory accessible from.
    //! \param extent The extent of the buffer.
    //! \return The newly allocated buffer.
    template<typename TElem, typename TIdx, typename TExtent, typename TDev>
    ALPAKA_FN_HOST auto allocMappedBuf(DevCpu const& host, TDev const& dev, TExtent const& extent = TExtent())
    {
        return trait::BufAllocMapped<TElem, Dim<TExtent>, TIdx, TDev>::allocMappedBuf(host, dev, extent);
    }

    //! Maps the buffer into the memory of the given device.
    //!
    //! \tparam TBuf The buffer type.
    //! \tparam TDev The device type.
    //! \param buf The buffer to map into the device memory.
    //! \param dev The device to map the buffer into.
    template<typename TBuf, typename TDev>
    ALPAKA_FN_HOST auto map(TBuf& buf, TDev const& dev) -> void
    {
        return trait::Map<TBuf, TDev>::map(buf, dev);
    }

    //! Unmaps the buffer from the memory of the given device.
    //!
    //! \tparam TBuf The buffer type.
    //! \tparam TDev The device type.
    //! \param buf The buffer to unmap from the device memory.
    //! \param dev The device to unmap the buffer from.
    template<typename TBuf, typename TDev>
    ALPAKA_FN_HOST auto unmap(TBuf& buf, TDev const& dev) -> void
    {
        return trait::Unmap<TBuf, TDev>::unmap(buf, dev);
    }

    //! Pins the buffer.
    //!
    //! \tparam TBuf The buffer type.
    //! \param buf The buffer to pin in the device memory.
    template<typename TBuf>
    ALPAKA_FN_HOST auto pin(TBuf& buf) -> void
    {
        return trait::Pin<TBuf>::pin(buf);
    }

    //! Unpins the buffer.
    //!
    //! \tparam TBuf The buffer type.
    //! \param buf The buffer to unpin from the device memory.
    template<typename TBuf>
    ALPAKA_FN_HOST auto unpin(TBuf& buf) -> void
    {
        return trait::Unpin<TBuf>::unpin(buf);
    }

    //! The pin state of the buffer.
    //!
    //! \tparam TBuf The buffer type.
    //! \param buf The buffer to get the pin state of.
    template<typename TBuf>
    ALPAKA_FN_HOST auto isPinned(TBuf const& buf) -> bool
    {
        return trait::IsPinned<TBuf>::isPinned(buf);
    }

    //! Prepares the buffer for non-blocking copy operations, e.g. pinning if
    //! non-blocking copy between a cpu and a cuda device is wanted
    //!
    //! \tparam TBuf The buffer type.
    //! \param buf The buffer to prepare in the device memory.
    template<typename TBuf>
    ALPAKA_FN_HOST auto prepareForAsyncCopy(TBuf& buf) -> void
    {
        return trait::PrepareForAsyncCopy<TBuf>::prepareForAsyncCopy(buf);
    }
} // namespace alpaka
