/**
 * This file is part of DSO.
 *
 * Copyright 2016 Technical University of Munich and Intel.
 * Developed by Jakob Engel <engelj at in dot tum dot de>,
 * for more information see <http://vision.in.tum.de/dso>.
 * If you use this code, please cite the respective publications as
 * listed on the above website.
 *
 * DSO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DSO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DSO. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdio>
#include <limits>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <utility>

#include "IOWrapper/OpenCV/DepthImageWrapper.h"
#include "IOWrapper/Output3DWrapper.h"
#include "util/FrameShell.h"

namespace dso::IOWrap {
DepthImageWrapper::DepthImageWrapper(std::filesystem::path output_directory,
                                     std::filesystem::path timestamp_output)
    : Output3DWrapper(), output_directory_(std::move(output_directory)) {
  timestamp_output_file_ = std::fopen(timestamp_output.c_str(), "w");
}

DepthImageWrapper::~DepthImageWrapper() noexcept {
  std::fclose(timestamp_output_file_);
}

void DepthImageWrapper::pushDepthImageFloat(MinimalImageF *image,
                                            FrameHessian *KF) {
  std::string basename(17, '\0');
  std::size_t str_len = std::snprintf(basename.data(), basename.size() - 1,
                                      "%010zu.tiff", count_);
  basename.resize(str_len);
  std::string filename = (output_directory_ / basename).string();

  cv::Mat_<float> depth_img(image->h, image->w);
  for (int v = 0; v < image->h; ++v) {
    for (int u = 0; u < image->w; ++u) {
      if (image->at(u, v) <= 0.01) {
        depth_img(v, u) = std::numeric_limits<float>::infinity();
      } else {
        depth_img(v, u) = 1.0F / image->at(u, v);
      }
    }
  }
  cv::imwrite(filename, depth_img);
  std::fprintf(timestamp_output_file_, "%.9lf\n", KF->shell->timestamp);
  ++count_;
}

} // namespace dso::IOWrap
