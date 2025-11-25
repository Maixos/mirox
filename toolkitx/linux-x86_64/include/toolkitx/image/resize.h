#pragma once

#include <opencv2/opencv.hpp>

namespace toolkitx::image {
    cv::Mat letterbox(
            const cv::Mat &image, const cv::Size &target_shape, int &pad_x, int &pad_y, float &scale,
            const cv::Scalar &scalar = cv::Scalar(114, 114, 114), bool auto_pad = false, bool scale_fill = false,
            bool scale_up = true, int stride = 32
    );

    class LetterboxResizer {
    public:
        explicit LetterboxResizer(
                bool scale_fill = false, bool scale_up = true, bool auto_pad = false, int stride = 32,
                cv::Scalar scalar = cv::Scalar(114, 114, 114)
        );

        void compute(const cv::Size &src_size, const cv::Size &dst_size);

        // 仿射变换：将原图像变换为目标尺寸
        void resize(const cv::Mat &src, cv::Mat &resized) const;

        // 将原图坐标系中的矩形转换为目标图坐标系
        [[nodiscard]] cv::Rect2f rect2dst(const cv::Rect2f &rect) const;

        // 将目标图坐标系中的矩形转换为原图坐标系
        [[nodiscard]] cv::Rect2f rect2ori(const cv::Rect2f &rect) const;

        // 将原图坐标系中的点转换为目标图坐标系
        [[nodiscard]] cv::Point2f point2dst(const cv::Point2f &pt) const;

        // 将目标图坐标系中的点转换为原图坐标系
        [[nodiscard]] cv::Point2f point2ori(const cv::Point2f &pt) const;

        // 将关键点从原图坐标系转换到目标图坐标系
        [[nodiscard]] std::vector<cv::Point2f> landmark2dst(const std::vector<cv::Point2f> &points) const;

        // 将关键点从目标图坐标系转换回原图坐标系
        [[nodiscard]] std::vector<cv::Point2f> landmark2ori(const std::vector<cv::Point2f> &points) const;

        // 恢复原图尺寸：将目标图像变换回原图尺寸
        [[nodiscard]] cv::Mat reverse(const cv::Mat &src) const;

        [[nodiscard]] float scale() const {
            return scale_;
        }

        [[nodiscard]] int pad_left() const {
            return pad_left_;
        }

        [[nodiscard]] int pad_right() const {
            return pad_right_;
        }

        [[nodiscard]] int pad_top() const {
            return pad_top_;
        }

        [[nodiscard]] int pad_bottom() const {
            return pad_bottom_;
        }

    private:
        int new_width_{}; // 缩放后的宽度
        int new_height_{}; // 缩放后的高度
        cv::Size src_size_{}; // 原图尺寸
        cv::Size dst_size_{}; // 目标图尺寸

        int stride_{}; // 步长，通常为 32，64
        bool scale_up_{}; // 是否允许放大
        bool auto_pad_{}; // 是否自动对齐填充
        bool scale_fill_{}; // 是否强制填充目标尺寸
        cv::Scalar scalar_{};

        float dw_{}; // 水平填充
        float dh_{}; // 垂直填充
        float scale_{}; // 缩放比例

        // 保存实际 padding（整数）
        int pad_left_{};
        int pad_right_{};
        int pad_top_{};
        int pad_bottom_{};
    };

    void test_letterbox_transform(std::string &image_path);
}
