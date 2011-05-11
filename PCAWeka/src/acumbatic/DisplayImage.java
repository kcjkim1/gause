package acumbatic;

import java.awt.image.BufferedImage;

//class DisplayImage
public class DisplayImage extends javax.swing.JFrame {

	// data member image
	java.awt.Image image;

	// data member displayimage
	java.awt.image.BufferedImage displayimage = new java.awt.image.BufferedImage(
			377, 233, java.awt.image.BufferedImage.TYPE_INT_ARGB);

	// data member graphics2d
	java.awt.Graphics2D graphics2d = displayimage.createGraphics();

	// constructor
	DisplayImage(String caminho) {
		image = java.awt.Toolkit.getDefaultToolkit().getImage(caminho);
		graphics2d.drawImage(image, 0, 0, this);
	}
	DisplayImage(BufferedImage img) {
		graphics2d.drawImage(img, 0, 0, this);
	}

	// paint
	public void paint(java.awt.Graphics a) {
		java.awt.Graphics2D b = (java.awt.Graphics2D) a;
		// graphics2d.drawImage(image,0,0,this);//uncommenting this makes it
		// work
		b.drawImage(displayimage, 0, 0, this);
	}
    public void run(BufferedImage imag){
    	DisplayImage displayimage = new DisplayImage(imag);
		displayimage.setSize(377, 233);
		displayimage.show();
    }
	static void main(String[] a) {
		/*DisplayImage displayimage = new DisplayImage();
		displayimage.setSize(377, 233);
		displayimage.show();*/
	}
}