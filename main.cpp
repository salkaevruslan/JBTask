#include <iostream>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cassert>

std::int64_t ans = 1e11; // int64_t тк ответ может переполнить int32_t, 1e11 точно больше ответа

struct Point final {
    std::int32_t x;
    std::int32_t y;

    bool operator==(const Point &other) const {
        return (x == other.x) && (y == other.y);
    }
};

bool compareByX(const Point left, const Point right) {
    if (left.x < right.x)
        return true;
    if (left.x > right.x)
        return false;
    return left.y <= right.y;
}

bool compareByY(const Point left, const Point right) {
    if (left.y < right.y)
        return true;
    if (left.y > right.y)
        return false;
    return left.x <= right.x;
}

void updateAns(const Point left, const Point right) {
    ans = std::min(ans, static_cast<std::int64_t>(left.x - right.x) * static_cast<std::int64_t>(left.x - right.x) +
                        static_cast<std::int64_t>(left.y - right.y) * static_cast<std::int64_t>(left.y - right.y));
}

/*
 * Воспользуемся методом Divide and Conquer. На очередном шаге алгоритма делим наши точки на 2 равных половине,
 * условно разделив их вертикальной линией. Рекурсивно запустимся от 2 половин,
 * посчитаем минимальное расстояние между точками внутри них.
 * Чтобы проверить расстояния между точками, находящимися в разных половинах, нам, очевидно,
 * достаточно рассмотреть лишь точки лежащие на расстояние не больше ранее найденного минимума от разделяющей линии.
 *
 * Тк мы передаем в функцию множество наших точек сортированное не только по горизонтали, но и по вертикали, ведь тогда,
 * получив отсортированный по Y массив центральных точек, можно обработать их за линию.
 * Несложно понять, что в любом прямоугольнике d x 2d нельзя разместить более 8 точек,
 * так чтобы расстояние между всеми было больше d. А для каждой точки, например справа,
 * нам интересны только лежащие справа на расстоянии не более ans
 * и по вертикали отличающиеся не более чем на ans, то это подходит.
 * Поэтому для каждой точки достаточно проверить 7 ближайших по вертикали(даже 7 ближайших сверху).
 *
 * Итого, на каждом шаге работает за линию. Тогда: T(n)=O(n)+2*T(n/2), значит работает за O(n log n)
 */
void countMinDist(std::vector<Point> points_by_x, std::vector<Point> points_by_y) {
    assert(points_by_x.size() == points_by_y.size());
    std::size_t len = points_by_x.size();
    if (len <= 1)
        return;

    /*
     * Разделение на 2 массива сортированных по горизонтали
     */
    auto first = begin(points_by_x);
    auto last = begin(points_by_x) + len / 2;
    std::vector<Point> points_by_x_left(first, last);
    first = last;
    last = end(points_by_x);
    std::vector<Point> points_by_x_right(first, last);

    /*
     * Разделение на 2 массива сортированных по вертикали
     */
    std::vector<Point> points_by_y_left;
    std::vector<Point> points_by_y_right;
    for (std::size_t i = 0; i < len; ++i) {
        if (compareByX(points_by_y[i], points_by_x[len / 2 - 1])) {
            points_by_y_left.push_back(points_by_y[i]);
        } else {
            points_by_y_right.push_back(points_by_y[i]);
        }
    }

    countMinDist(points_by_x_left, points_by_y_left);
    countMinDist(points_by_x_right, points_by_y_right);

    /*
     * Нахождение 'центральных точек'
     */
    std::int32_t middle = points_by_x[len / 2 - 1].x;
    std::vector<Point> middle_points;
    for (std::size_t i = 0; i < len; ++i) {
        if ((points_by_y[i].x - middle) * (points_by_y[i].x - middle) <= ans) {
            middle_points.push_back(points_by_y[i]);
        }
    }

    /*
     * Проверка 'центральных точек'
     */
    for (std::size_t i = 0; i < middle_points.size(); ++i) {
        for (std::size_t j = i + 1; j < std::min(i + 8, middle_points.size()); ++j) {
            updateAns(middle_points[i], middle_points[j]);
        }
    }
}

int main() {
    std::int32_t n, x, y;
    std::cin >> n;
    std::vector<Point> points_by_x;
    std::vector<Point> points_by_y;
    for (std::size_t i = 0; i < n; ++i) {
        std::cin >> x >> y;
        points_by_x.push_back({x, y});
        points_by_y.push_back({x, y});
    }
    sort(begin(points_by_x), end(points_by_x), compareByX);
    sort(begin(points_by_y), end(points_by_y), compareByY);
    for (std::size_t i = 0; i < n - 1; ++i) {
        if (points_by_x[i] == points_by_x[i + 1]) {
            std::cout << 0;
            return 0;
        }
    }
    countMinDist(points_by_x, points_by_y);
    std::cout << ans;
}