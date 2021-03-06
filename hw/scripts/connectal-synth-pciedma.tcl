source "board.tcl"

proc fpgamake_altera_synth_qsys {core_name core_version ip_name} {
    global ipdir boardname partname

    exec -ignorestderr -- ip-generate \
            --project-directory=$ipdir/$boardname                            \
            --search-path=$ipdir/../../sonic-lite/qsys/,$                    \
            --output-directory=$ipdir/$boardname/synthesis                   \
            --file-set=QUARTUS_SYNTH                                         \
            --report-file=html:$ipdir/$boardname/$ip_name.html               \
            --report-file=sopcinfo:$ipdir/$boardname/$ip_name.sopcinfo       \
            --report-file=cmp:$ipdir/$boardname/$ip_name.cmp                 \
            --report-file=qip:$ipdir/$boardname/synthesis/$ip_name.qip       \
            --report-file=svd:$ipdir/$boardname/synthesis/$ip_name.svd       \
            --report-file=regmap:$ipdir/$boardname/synthesis/$ip_name.regmap \
            --report-file=xml:$ipdir/$boardname/$ip_name.xml                 \
            --system-info=DEVICE_FAMILY=StratixV                             \
            --system-info=DEVICE=$partname                                   \
            --system-info=DEVICE_SPEEDGRADE=2_H2                             \
            --language=VERILOG                                               \
            --component-file=$core_name                                      \
            --output-name=$ip_name
}

fpgamake_altera_synth_qsys ../qsys/pcie_dma.qsys 14.0 pcie_dma
