/*=========================================================================
*
*  Copyright Insight Software Consortium
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/

#ifndef itkBorderReplicatingImageNeighborhoodPixelAccessPolicy_h
#define itkBorderReplicatingImageNeighborhoodPixelAccessPolicy_h

#include "itkIndex.h"
#include "itkOffset.h"
#include "itkSize.h"

namespace itk
{
namespace Experimental
{

/**
 * \class BorderReplicatingImageNeighborhoodPixelAccessPolicy
 * ImageNeighborhoodPixelAccessPolicy class for ShapedImageNeighborhoodRange.
 * Allows getting and setting the value of a pixel, located in a specified
 * neighborhood location, at a specified offset. Uses "border replication" as
 * extrapolation method for pixels outside the image border.
 *
 * \see ShapedNeighborhoodIterator
 * \see ZeroFluxNeumannBoundaryCondition
 * \ingroup ImageIterators
 * \ingroup ITKCommon
 */
template <typename TImage>
class BorderReplicatingImageNeighborhoodPixelAccessPolicy final
{
private:
  using NeighborhoodAccessorFunctorType = typename TImage::NeighborhoodAccessorFunctorType;
  using PixelType = typename TImage::PixelType;
  using InternalPixelType = typename TImage::InternalPixelType;

  using ImageDimensionType = typename TImage::ImageDimensionType;
  static constexpr ImageDimensionType ImageDimension = TImage::ImageDimension;

  using IndexType = Index<ImageDimension>;
  using OffsetType = Offset<ImageDimension>;
  using ImageSizeType = Size<ImageDimension>;
  using ImageSizeValueType = SizeValueType;

  // Index value to the image buffer, indexing the current pixel.
  const IndexValueType m_PixelIndexValue;

  // A reference to the accessor of the image.
  const NeighborhoodAccessorFunctorType& m_NeighborhoodAccessor;


  // Private helper function. Clamps the index value between the interval
  // [0 .. imageSizeValue>.
  static IndexValueType GetClampedIndexValue(
    const IndexValueType indexValue,
    const ImageSizeValueType imageSizeValue) ITK_NOEXCEPT
  {
    return (indexValue <= 0) ? 0 :
      (static_cast<ImageSizeValueType>(indexValue) < imageSizeValue) ? indexValue : static_cast<IndexValueType>(imageSizeValue - 1);
  }

  // Private helper function. Calculates and returns the index value of the
  // current pixel within the image buffer.
  static IndexValueType CalculatePixelIndexValue(
    const ImageSizeType& imageSize,
    const OffsetType& offsetTable,
    const IndexType& pixelIndex) ITK_NOEXCEPT
  {
    IndexValueType result = 0;

    for (ImageDimensionType i = 0; i < ImageDimension; ++i)
    {
      result += GetClampedIndexValue(pixelIndex[i], imageSize[i]) * offsetTable[i];
    }
    return result;
  }

public:
  // Deleted member functions:
  BorderReplicatingImageNeighborhoodPixelAccessPolicy() = delete;
  BorderReplicatingImageNeighborhoodPixelAccessPolicy& operator=(const BorderReplicatingImageNeighborhoodPixelAccessPolicy&) = delete;

  // Explicitly-defaulted functions:
  ~BorderReplicatingImageNeighborhoodPixelAccessPolicy() = default;
  BorderReplicatingImageNeighborhoodPixelAccessPolicy(
    const BorderReplicatingImageNeighborhoodPixelAccessPolicy&) ITK_NOEXCEPT = default;

  /** Constructor called directly by the pixel proxy of
   * ShapedImageNeighborhoodRange. */
  BorderReplicatingImageNeighborhoodPixelAccessPolicy(
    const ImageSizeType& imageSize,
    const OffsetType& offsetTable,
    const NeighborhoodAccessorFunctorType& neighborhoodAccessor,
    const IndexType& pixelIndex) ITK_NOEXCEPT
    :
  m_PixelIndexValue{ CalculatePixelIndexValue(imageSize, offsetTable, pixelIndex) },
  m_NeighborhoodAccessor(neighborhoodAccessor)
  {
  }

  /** Retrieves the pixel value from the image buffer, at the current
   * index value.  */
  PixelType GetPixelValue(const InternalPixelType* const imageBufferPointer) const ITK_NOEXCEPT
  {
    return m_NeighborhoodAccessor.Get(imageBufferPointer + m_PixelIndexValue);
  }

  /** Sets the value of the image buffer at the current index value to the
   * specified value.  */
  void SetPixelValue(InternalPixelType* const imageBufferPointer, const PixelType& pixelValue) const ITK_NOEXCEPT
  {
    m_NeighborhoodAccessor.Set(imageBufferPointer + m_PixelIndexValue, pixelValue);
  }
};

} // namespace Experimental
} // namespace itk

#endif