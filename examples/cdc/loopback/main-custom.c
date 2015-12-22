// This file is included from ../common/main.c
// -----------------------------------------------------------------------

BOOL cdcuser_set_line_rate(DWORD baud_rate) {
	return TRUE;
}

void cdcuser_receive_data(BYTE* data, WORD length) {
        WORD i;
        for (i=0; i < length ; ++i) {
		cdc_queue_data(data[i]);
	}
	cdc_send_queued_data();
}
