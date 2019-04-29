// Copyright (C) 2018-2019 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <utility>

#include "ObjectDetectionProcessor.hpp"

using namespace std;

class SSDObjectDetectionProcessor : public ObjectDetectionProcessor {
protected:
    std::map<std::string, std::list<DetectedObject>> processResult(std::vector<std::string> files) {
        std::map<std::string, std::list<DetectedObject>> detectedObjects;

        std::string firstOutputName = this->outInfo.begin()->first;
        const auto detectionOutArray = inferRequest.GetBlob(firstOutputName);
        const float *box = detectionOutArray->buffer().as<float*>();

        const size_t maxProposalCount = outputDims[1];
        const size_t objectSize = outputDims[0];

        for (size_t b = 0; b < batch; b++) {
            string fn = files[b];
            std::list<DetectedObject> dr = std::list<DetectedObject>();
            detectedObjects.insert(std::pair<std::string, std::list<DetectedObject>>(fn, dr));
        }

        for (size_t i = 0; i < maxProposalCount; i++) {
            float image_id = box[i * objectSize + 0];
            float label = box[i * objectSize + 1];
            float confidence = box[i * objectSize + 2];
            float xmin = box[i * objectSize + 3] * inputDims[0];
            float ymin = box[i * objectSize + 4] * inputDims[1];
            float xmax = box[i * objectSize + 5] * inputDims[0];
            float ymax = box[i * objectSize + 6] * inputDims[1];

            if (image_id < 0 /* better than check == -1 */) {
                break;  // Finish
            }

            detectedObjects[files[static_cast<size_t>(image_id)]].push_back(
                DetectedObject(static_cast<int>(label), xmin, ymin, xmax, ymax, confidence));
        }

        return detectedObjects;
    }

public:
    SSDObjectDetectionProcessor(const std::string& flags_m, const std::string& flags_d, const std::string& flags_i, const std::string& subdir, int flags_b,
            double threshold,
            InferencePlugin plugin, CsvDumper& dumper,
            const std::string& flags_a, const std::string& classes_list_file) :

                ObjectDetectionProcessor(flags_m, flags_d, flags_i, subdir, flags_b, threshold,
                        plugin, dumper, flags_a, classes_list_file, PreprocessingOptions(false, ResizeCropPolicy::Resize), true) { }
};
