library verilog;
use verilog.vl_types.all;
entity definitivo is
    port(
        puerta          : out    vl_logic;
        \on/off\        : in     vl_logic;
        T5              : in     vl_logic;
        T4              : in     vl_logic;
        T3              : in     vl_logic;
        T2              : in     vl_logic;
        T1              : in     vl_logic;
        p1              : out    vl_logic;
        p2              : out    vl_logic;
        p3              : out    vl_logic;
        p4              : out    vl_logic;
        T6              : in     vl_logic;
        s1              : in     vl_logic;
        s2              : in     vl_logic;
        s3              : in     vl_logic;
        s5              : in     vl_logic;
        s6              : in     vl_logic;
        s7              : in     vl_logic;
        s8              : in     vl_logic;
        s9              : in     vl_logic;
        s0              : in     vl_logic;
        s4              : in     vl_logic
    );
end definitivo;
