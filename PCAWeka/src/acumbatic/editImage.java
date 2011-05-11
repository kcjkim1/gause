package acumbatic;

public class editImage {

	/**
	 * @param args
	 */
	
	
	public void openIMG(String src) {

		if (src != null) {
			foto f = new foto(src);
			f.setSize(600, 600);
			f.setVisible(true);
		}

	}
	public static void main(String[] args) {
		foto f = new foto("/Users/jovanoasis/Desktop/4927.jpg");
		f.setSize(600, 600);
		f.setVisible(true);
	}

}
