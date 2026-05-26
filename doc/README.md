# HALO - CYBER ACCESS ENGINE

## Mô tả dự án
HALO là một engine phân tích truy cập được xây dựng bằng C++. Hệ thống được thiết kế để giải quyết bài toán xử lý log sự kiện an ninh mạng quy mô lớn, giúp tổ chức dữ liệu hiệu quả và thực hiện tìm kiếm với tốc độ cao. Chương trình có khả năng làm việc trên tập dữ liệu lên đến hàng triệu dòng.

## Chức năng chính
Hệ thống hiện tại hỗ trợ các tính năng khai thác dữ liệu sau:
* Liệt kê hành trình truy xuất tài nguyên của một người dùng cụ thể (Device -> App -> Resource) trong một khoảng thời gian cho trước.
* Liệt kê hành trình truy xuất đối với một tài nguyên cụ thể (User -> Device -> App) trong một khoảng thời gian cho trước.
* Thống kê Top 10 tài nguyên được truy xuất nhiều nhất trong hệ thống.

## Chi tiết kỹ thuật
* **Cấu trúc dữ liệu:** Xây dựng từ đầu các cấu trúc mảng động `LogArray`, `StringArray` và Bảng băm `HashTable` sử dụng cấu trúc `structure`, con trỏ, và danh sách liên kết.
* **Quản lý bộ nhớ:** Tất cả dữ liệu được cấp phát động đều có cơ chế thu hồi tự động qua các hàm hủy (destructor) để tránh rò rỉ bộ nhớ.
* **Tối ưu I/O:** Sử dụng trực tiếp các API đọc file cấp thấp của hệ điều hành (`ReadFile` trên Windows, `read` trên POSIX) để đọc file theo chunk (1MB).
* **Thuật toán cốt lõi:**
  * Sử dụng thuật toán băm djb2 cho `HashTable`.
  * Sắp xếp log bằng `QuickSort`
  * Tìm kiếm theo thời gian bằng Tìm kiếm nhị phân (Binary Search).
## Yêu cầu hệ thống
* Trình biên dịch C++ hỗ trợ chuẩn **C++17** trở lên (do sử dụng thư viện `<filesystem>`).
* Hệ điều hành: Windows hoặc các hệ thống dựa trên POSIX (Linux, macOS).
* Dữ liệu đầu vào: Các log file có định dạng `.csv`.
## Hướng dẫn cài đặt và biên dịch
### Biên dịch trên Windows (sử dụng MSVC hoặc MinGW)
```bash
g++ -std=c++17 -O3 -o halo_engine.exe src/main.cpp
```
### Biên dịch trên Linux/MacOS
```bash
g++ -std=c++17 -O3 -o halo_engine src/main.cpp
```
## Hướng dẫn sử dụng
* Đặt các file log dữ liệu .csv vào cùng thư mục với file thực thi hoặc trong các thư mục con. File `.csv` cần có dòng tiêu đề và các cột tương ứng: user_id, device_id, app_id, resource_id, event_type, location, timestamp.  
* Chạy file thực thi (halo_engine.exe hoặc ./halo_engine).
* Chương trình sẽ tự động quét thư mục và hiển thị danh sách các file `.csv` tìm thấy.
* Nhập số thứ tự để chọn file log cần nạp vào bộ nhớ.
* Sử dụng Menu dòng lệnh (CLI) để nhập các truy vấn (ID, khoảng thời gian dạng Epoch time).
* Nhập 0 để thoát chương trình và giải phóng bộ nhớ.
## cấu trúc thư mục
```
/
├── src/                # Chứa toàn bộ mã nguồn chương trình.
│   └── main.cpp        # Mã nguồn chính của HALO Engine
├── release/            # Chứa file thực thi được build ở chế độ release.
├── doc/                # Báo cáo đánh giá mức hoàn thành đồ án, thiết kế hệ thống.
└── README.md           # Tài liệu hướng dẫn này
```
