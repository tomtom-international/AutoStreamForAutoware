/*
 * Copyright © 2021 TomTom NV. All rights reserved.
 *
 * This software is the proprietary copyright of TomTom NV and its subsidiaries and may be
 * used for internal evaluation purposes or commercial use strictly subject to separate
 * license agreement between you and TomTom NV. If you are the licensee, you are only permitted
 * to use this software in accordance with the terms of your license agreement. If you are
 * not the licensee, you are not authorized to use this software in any manner and should
 * immediately return or destroy it.
 */

#include "AutoStreamMapConverter/DataTypes.hpp"

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

bool CAutoStreamArcData::empty() const noexcept
{
  return mLaneBorders.empty() && mLaneMetaData.empty();
}

bool CAutoStreamArcData::isValid() const noexcept
{
  return mLaneBorders.size() - 1 == mLaneMetaData.size();
}

size_t CAutoStreamArcData::size() const noexcept
{
  return mLaneMetaData.size();
}
}
}
}