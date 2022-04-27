/// Returns the most significant bit of __bits, depending on __mask and __shift
/// Ex: bits = 0x801A; mask = 0xF000; shift = 12; will return 8
template <typename T>
inline std::uint8_t get_msbit(const T& bits, const T& mask, const int& shift) {
    return (bits & mask) >> shift;
}
