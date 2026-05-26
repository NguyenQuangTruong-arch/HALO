# HALO - CYBER ACCESS ENGINE

## Mô tả dự án
[cite_start]HALO là một engine phân tích truy cập được xây dựng bằng C++[cite: 8, 11]. [cite_start]Hệ thống được thiết kế để giải quyết bài toán xử lý log sự kiện an ninh mạng quy mô lớn, giúp tổ chức dữ liệu hiệu quả và thực hiện tìm kiếm với tốc độ cao[cite: 6, 8]. [cite_start]Dự án có khả năng làm việc trên tập dữ liệu lên đến hàng triệu dòng mà không gặp sự cố crash, đồng thời tối ưu hóa thời gian trả kết quả[cite: 66, 69].

## Chức năng chính
Hệ thống hiện tại hỗ trợ các tính năng khai thác dữ liệu sau:
* [cite_start]Tìm kiếm nhanh trên tập dữ liệu lớn[cite: 12].
* [cite_start]Phân tích hoạt động truy cập[cite: 13].
* [cite_start]Liệt kê hành trình truy xuất tài nguyên của một người dùng cụ thể (Device -> App -> Resource) trong một khoảng thời gian cho trước[cite: 38].
* [cite_start]Liệt kê hành trình truy xuất đối với một tài nguyên cụ thể (User -> Device -> App) trong một khoảng thời gian cho trước[cite: 39].
* [cite_start]Thống kê Top 10 tài nguyên được truy xuất nhiều nhất trong hệ thống[cite: 40].

## Chi tiết kỹ thuật
Hệ thống được phát triển với các ràng buộc kỹ thuật khắt khe nhằm tối ưu hóa hiệu suất và quản lý bộ nhớ ở mức thấp:
* [cite_start]**Không sử dụng STL Containers:** Hoàn toàn không sử dụng các lớp container có sẵn của thư viện chuẩn như `vector`, `map`, `unordered_map`, `set`[cite: 80].
* [cite_start]**Cấu trúc dữ liệu tự định nghĩa:** Xây dựng từ đầu các cấu trúc mảng động (`LogArray`, `StringArray`) và Bảng băm (`HashTable`) sử dụng cấu trúc `structure`, con trỏ, và danh sách liên kết[cite: 75, 77, 78].
* [cite_start]**Quản lý bộ nhớ:** Tất cả dữ liệu được cấp phát động đều có cơ chế thu hồi tự động qua các hàm hủy (destructor) để tránh rò rỉ bộ nhớ[cite: 85].
* **Tối ưu I/O:** Sử dụng trực tiếp các API đọc file cấp thấp của hệ điều hành (`ReadFile` trên Windows, `read` trên POSIX) để đọc file theo chunk (1MB), vượt qua giới hạn tốc độ của `std::ifstream`.
* **Thuật toán cốt lõi:** * Sử dụng thuật toán băm djb2 cho `HashTable`.
  * Sắp xếp log bằng `QuickSort`.
  * Tìm kiếm theo thời gian bằng Tìm kiếm nhị phân (Binary Search).

## Yêu cầu hệ thống
* Trình biên dịch C++ hỗ trợ chuẩn **C++17** trở lên (bắt buộc do sử dụng thư viện `<filesystem>`).
* Hệ điều hành: Windows hoặc các hệ thống dựa trên POSIX (Linux, macOS).
* [cite_start]Dữ liệu đầu vào: Các log file có định dạng CSV[cite: 17].

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
* Đặt các file log dữ liệu .csv vào cùng thư mục với file thực thi hoặc trong các thư mục con. File CSV cần có dòng tiêu đề và các cột tương ứng: user_id, device_id, app_id, resource_id, event_type, location, timestamp.  
* Chạy file thực thi (halo_engine.exe hoặc ./halo_engine).
* Chương trình sẽ tự động quét thư mục và hiển thị danh sách các file CSV tìm thấy.
* Nhập số thứ tự để chọn file log cần nạp vào bộ nhớ.
* Sử dụng Menu dòng lệnh (CLI) để nhập các truy vấn (ID, khoảng thời gian dạng Epoch time) và nhận kết quả.
* Nhập 0 để thoát chương trình và giải phóng bộ nhớ.
## cấu trúc thư mục
/
├── src/                # Chứa toàn bộ mã nguồn chương trình[cite: 91].
│   └── main.cpp        # Mã nguồn chính của HALO Engine
├── release/            # Chứa file thực thi được build ở chế độ release[cite: 92].
├── doc/                # Báo cáo đánh giá mức hoàn thành đồ án, thiết kế hệ thống[cite: 93].
└── README.md           # Tài liệu hướng dẫn này
