public class Frame {
	public byte[] buf;
	public byte[] data;
	public int size;
	public static int hlen = 3;	// header length
	
	public Frame(byte num, byte[] data, int size) {
		this.data = data;
		buf = new byte[1024];
		buf[0] = num;
		if (size < 0) size = 0;
		this.size = size;
		buf[1] = (byte) ((size >>> 8) & 0xff);
		buf[2] = (byte) (size & 0xff);
		for (int i = hlen; i < size + hlen; i++) {
			buf[i] = data[i-hlen];
		}
	}
	
	public Frame(byte[] buf) {
		this.buf = buf;
		size = 0x0000ff00 & (buf[1] << 8);
		size += 0x000000ff & buf[2];
		data = new byte[size];
		for (int i = hlen; i < size + hlen; i++) {
			data[i-hlen] = buf[i];
		}
	}
}
