package acumbatic;

import java.awt.Color;
import java.awt.RenderingHints;
import java.awt.Transparency;
import java.awt.color.ColorSpace;
import java.awt.color.ICC_ColorSpace;
import java.awt.color.ICC_Profile;
import java.awt.image.BufferedImage;
import java.awt.image.ColorConvertOp;
import java.awt.image.ColorModel;
import java.awt.image.renderable.ParameterBlock;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.imageio.ImageIO;
import javax.media.jai.*;

import weka.attributeSelection.PrincipalComponents;
import weka.attributeSelection.Ranker;
import weka.core.Attribute;
import weka.core.FastVector;
import weka.core.Instance;
import weka.core.Instances;
import weka.core.converters.ArffSaver;
import weka.filters.Filter;

import com.jhlabs.image.GaussianFilter;

public class findPCA {

	private static float[] rgb2hsv(int r, int g, int b) {


		int min; // Min. value of RGB
		int max; // Max. value of RGB
		int delMax; // Delta RGB value

		if (r > g) {
			min = g;
			max = r;
		} else {
			min = r;
			max = g;
		}
		if (b > max)
			max = b;
		if (b < min)
			min = b;

		delMax = max - min;

		float H = 0, S;
		float V = max;

		if (delMax == 0) {
			H = 0;
			S = 0;
		} else {
			S = delMax / 255f;
			if (r == max)
				H = ((g - b) / (float) delMax) * 60;
			else if (g == max)
				H = (2 + (b - r) / (float) delMax) * 60;
			else if (b == max)
				H = (4 + (r - g) / (float) delMax) * 60;
		}

		float[] hsv = new float[3];
		hsv[0] = (float) (H);
		hsv[1] = (float) (S * 100);
		hsv[2] = (float) (V * 100);

		return hsv;
	}

	private static Double[] rgb2ycbcr(int r, int g, int b) {


		Double[] ycbcr = new Double[3];

		ycbcr[0] = (0.299*r) + (0.587*g) + (0.114*b);
		ycbcr[1] = (b-ycbcr[0])*0.564 +128;
		ycbcr[2]= (r-ycbcr[0])*0.713 +128;


		return ycbcr;
	}

	   public static BufferedImage toXYZ(BufferedImage bufferedImage){        
		            ColorSpace cs = ColorSpace.getInstance(ColorSpace.CS_CIEXYZ);
		            ColorConvertOp op = new ColorConvertOp(cs, null);
		            bufferedImage = op.filter(bufferedImage, null);
		            System.out.println(bufferedImage.getColorModel());
		            return bufferedImage;
	   }
	private static float[] rgb2xyz(int R, int G, int B) {

		float r, g, b, X, Y, Z;

		r = R / 255.f; // R 0..1
		g = G / 255.f; // G 0..1
		b = B / 255.f; // B 0..1

		if (r <= 0.04045)
			r = r / 12;
		else
			r = (float) Math.pow((r + 0.055) / 1.055, 2.4);

		if (g <= 0.04045)
			g = g / 12;
		else
			g = (float) Math.pow((g + 0.055) / 1.055, 2.4);

		if (b <= 0.04045)
			b = b / 12;
		else
			b = (float) Math.pow((b + 0.055) / 1.055, 2.4);

		X = 0.436052025f * r + 0.385081593f * g + 0.143087414f * b;
		Y = 0.222491598f * r + 0.71688606f * g + 0.060621486f * b;
		Z = 0.013929122f * r + 0.097097002f * g + 0.71418547f * b;

		float[] xyz = new float[3];

		xyz[0] = (float) (255 * X + .5);
		xyz[1] = (float) (255 * Y + .5);
		xyz[2] = (float) (255 * Z + .5);

		return xyz;
	}
	
	
	private static float[] rgb2CIEXYZ(int r, int g, int b) {

		float[] xyz = new float[3];
		xyz[0] = 0.412453f * r + 0.357580f * g + 0.180423f * b;
		xyz[1] = 0.212671f * r + 0.715160f * g + 0.072169f * b;
		xyz[2] = 0.019334f * r + 0.119193f * g + 0.950227f * b;

		
		

		return xyz;
	}

	private static float[] rgb2hsl(int r, int g, int b) {

		float var_R = (r / 255f);
		float var_G = (g / 255f);
		float var_B = (b / 255f);

		float var_Min; // Min. value of RGB
		float var_Max; // Max. value of RGB
		float del_Max; // Delta RGB value

		if (var_R > var_G) {
			var_Min = var_G;
			var_Max = var_R;
		} else {
			var_Min = var_R;
			var_Max = var_G;
		}

		if (var_B > var_Max)
			var_Max = var_B;
		if (var_B < var_Min)
			var_Min = var_B;

		del_Max = var_Max - var_Min;

		float H = 0, S, L;
		L = (var_Max + var_Min) / 2f;

		if (del_Max == 0) {
			H = 0;
			S = 0;
		} // gray
		else { // Chroma
			if (L < 0.5)
				S = del_Max / (var_Max + var_Min);
			else
				S = del_Max / (2 - var_Max - var_Min);

			float del_R = (((var_Max - var_R) / 6f) + (del_Max / 2f)) / del_Max;
			float del_G = (((var_Max - var_G) / 6f) + (del_Max / 2f)) / del_Max;
			float del_B = (((var_Max - var_B) / 6f) + (del_Max / 2f)) / del_Max;

			if (var_R == var_Max)
				H = del_B - del_G;
			else if (var_G == var_Max)
				H = (1 / 3f) + del_R - del_B;
			else if (var_B == var_Max)
				H = (2 / 3f) + del_G - del_R;
			if (H < 0)
				H += 1;
			if (H > 1)
				H -= 1;
		}

		float[] hsl = new float[3];
		hsl[0] = (float) (360 * H);
		hsl[1] = (float) (S * 100);
		hsl[2] = (float) (L * 100);

		return hsl;
	}
	private static float[] rgb2CIELab(int r, int g, int b){
		
		float[] XYZ = rgb2CIEXYZ(r, g, b);
		float[] Lab = new float[3];
		Double x,y,z;
		Double eps = 0.008856;
		Double k   = 0.137931034;
		 x= Double.valueOf(XYZ[0]/255);//0..1
		 y= Double.valueOf(XYZ[1]/255);//0..1
		 z= Double.valueOf(XYZ[2]/255);//0..1
		
		 
		 Double tAux = x/0.950456;
		 Double tAux2 = z/1.088754;
		 Double f1,f2,f3;
		 double raiz =(double) 1/3;
		 
		 //Calculating L
		 
		 if (y > eps){
			 Lab[0]=(float) (116 * Math.pow(y,raiz)) - 16;
			 f2 = Math.pow(y,raiz);
		 }else{
			 Lab[0]= (float) (903.3*y);
			 f2 =  ((7.787*y) + 0.137931034);
		 }
		 
		 //Calculating a 
		 if(tAux > eps){
			 f1 = Math.pow(tAux,raiz);
		 }else{
			 f1 = ((7.787*tAux) + 0.137931034);
		 }
		 if(tAux2 > eps){
			 f3 = Math.pow(tAux2,raiz);
		 }else{
			 f3 = ((7.787*tAux2) + k);
		 }	 

		 Lab[1] =(float) (500 *(f1 - f2 ) + 128);
		
		 //Calculating b
		 Lab[2] = (float)(200 *(f2 - f3 ) + 128);
		
		return Lab;
	}
	private static float[] rgb2lab(int R, int G, int B) {

		float r, g, b, X, Y, Z, fx, fy, fz, xr, yr, zr;
		float Ls, as, bs;
		float eps = 216.f / 24389.f;
		float k = 24389.f / 27.f;

		float Xr = 0.964221f; // reference white D50
		float Yr = 1.0f;
		float Zr = 0.825211f;

		// RGB to XYZ
		r = R / 255.f; // R 0..1
		g = G / 255.f; // G 0..1
		b = B / 255.f; // B 0..1

		// assuming sRGB (D65)
		if (r <= 0.04045)
			r = r / 12;
		else
			r = (float) Math.pow((r + 0.055) / 1.055, 2.4);

		if (g <= 0.04045)
			g = g / 12;
		else
			g = (float) Math.pow((g + 0.055) / 1.055, 2.4);

		if (b <= 0.04045)
			b = b / 12;
		else
			b = (float) Math.pow((b + 0.055) / 1.055, 2.4);

		X = 0.436052025f * r + 0.385081593f * g + 0.143087414f * b;
		Y = 0.222491598f * r + 0.71688606f * g + 0.060621486f * b;
		Z = 0.013929122f * r + 0.097097002f * g + 0.71418547f * b;

		// XYZ to Lab
		xr = X / Xr;
		yr = Y / Yr;
		zr = Z / Zr;

		if (xr > eps)
			fx = (float) Math.pow(xr, 1 / 3.);
		else
			fx = (float) ((k * xr + 16.) / 116.);

		if (yr > eps)
			fy = (float) Math.pow(yr, 1 / 3.);
		else
			fy = (float) ((k * yr + 16.) / 116.);

		if (zr > eps)
			fz = (float) Math.pow(zr, 1 / 3.);
		else
			fz = (float) ((k * zr + 16.) / 116);

		Ls = (116 * fy) - 16;
		as = 500 * (fx - fy);
		bs = 200 * (fy - fz);

		float[] lab = new float[3];

		lab[0] = (float) (2.55 * Ls + .5);
		lab[1] = (float) (as + .5);
		lab[2] = (float) (bs + .5);

		return lab;
	}

	private static float[] rgb2luv(int R, int G, int B) {

		float r, g, b, X, Y, Z, yr;
		float L;
		float eps = 216.f / 24389.f;
		float k = 24389.f / 27.f;

		float Xr = 0.964221f; // reference white D50
		float Yr = 1.0f;
		float Zr = 0.825211f;

		// RGB to XYZ

		r = R / 255.f; // R 0..1
		g = G / 255.f; // G 0..1
		b = B / 255.f; // B 0..1

		// assuming sRGB (D65)
		if (r <= 0.04045)
			r = r / 12;
		else
			r = (float) Math.pow((r + 0.055) / 1.055, 2.4);

		if (g <= 0.04045)
			g = g / 12;
		else
			g = (float) Math.pow((g + 0.055) / 1.055, 2.4);

		if (b <= 0.04045)
			b = b / 12;
		else
			b = (float) Math.pow((b + 0.055) / 1.055, 2.4);

		X = 0.436052025f * r + 0.385081593f * g + 0.143087414f * b;
		Y = 0.222491598f * r + 0.71688606f * g + 0.060621486f * b;
		Z = 0.013929122f * r + 0.097097002f * g + 0.71418547f * b;

		// XYZ to Luv

		float u, v, u_, v_, ur_, vr_;

		u_ = 4 * X / (X + 15 * Y + 3 * Z);
		v_ = 9 * Y / (X + 15 * Y + 3 * Z);

		ur_ = 4 * Xr / (Xr + 15 * Yr + 3 * Zr);
		vr_ = 9 * Yr / (Xr + 15 * Yr + 3 * Zr);

		yr = Y / Yr;

		if (yr > eps)
			L = (float) (116 * Math.pow(yr, 1 / 3.) - 16);
		else
			L = k * yr;

		u = 13 * L * (u_ - ur_);
		v = 13 * L * (v_ - vr_);

		float[] luv = new float[3];

		luv[0] = (float) (2.55 * L + .5);
		luv[1] = (float) (u + .5);
		luv[2] = (float) (v + .5);

		return luv;
	}

	public static String PixelARGB(int pixel) {
		int alpha = (pixel >> 24) & 0xff;
		int red = (pixel >> 16) & 0xff;
		int green = (pixel >> 8) & 0xff;
		int blue = (pixel) & 0xff;
		return "alpha: " + alpha + ", red: " + red + ", green: " + green
				+ ", blue: " + blue;
	}

	/*
	 * public static void JavaWalkBufferedImageTest1( ArrayList<double[]> vals)
	 * { try { // get the BufferedImage, using the ImageIO class BufferedImage
	 * image = ImageIO.read(new File("/Users/jovanoasis/Desktop/Recent 2.jpg"));
	 * System.out.println(PixelARGB(image.getRGB(1, 1))); } catch (IOException
	 * e) { System.err.println(e.getMessage()); } }
	 */
	@SuppressWarnings("restriction")
	public static Instances createArffFile(String path,List<String> imgNames,String arrfFileName) throws Exception {
		System.out.println("creating "+ arrfFileName);
		
		FastVector atts;
		FastVector attVals;
		Instances data;
		double[] vals = null;

		// 1. set up attributes
		atts = new FastVector();
		// - Color space

		// RGB
		atts.addElement(new Attribute("RGB-R"));
		atts.addElement(new Attribute("RGB-G"));
		atts.addElement(new Attribute("RGB-B"));

		// HSV
		atts.addElement(new Attribute("HSV-H"));
		atts.addElement(new Attribute("HSV-S"));
		atts.addElement(new Attribute("HSV-V"));

		// YCbCr
		atts.addElement(new Attribute("YCbCr-Y"));
		atts.addElement(new Attribute("YCbCr-Cb"));
		atts.addElement(new Attribute("YCbCr-Cr"));

		// XYZ
		atts.addElement(new Attribute("XYZ-X"));
		atts.addElement(new Attribute("XYZ-Y"));
		atts.addElement(new Attribute("XYZ-Z"));

		// HSL
		atts.addElement(new Attribute("HSL-H"));
		atts.addElement(new Attribute("HSL-S"));
		atts.addElement(new Attribute("HSL-L"));

		// Lab
		atts.addElement(new Attribute("Lab-L"));
		atts.addElement(new Attribute("Lab-a"));
		atts.addElement(new Attribute("Lab-b"));

		// Luv
		atts.addElement(new Attribute("Luv-L"));
		atts.addElement(new Attribute("Luv-u"));
		atts.addElement(new Attribute("Luv-v"));

		// Positions
		atts.addElement(new Attribute("horizontal"));
		atts.addElement(new Attribute("vertical"));

		// Classe P or N
		// - nominal
		attVals = new FastVector();

		// add array de valores

		attVals.addElement("P");
		attVals.addElement("N");

		Attribute classe = new Attribute("Classe", attVals);
		atts.addElement(classe);

		// 2. create Instances object
		data = new Instances(arrfFileName, atts, 0);
		data.setClassIndex(23);

		// 3. fill with data

		// Opening the original Image
		for (String imgName : imgNames) {
			System.out.println("Opening "+ imgName );
				BufferedImage image = gaussFilter(ImageIO.read(new File(path
						+ imgName + ".jpg")), 3);

		// Opening the mask Image
		ParameterBlock parameterBlock = new ParameterBlock();
		parameterBlock
				.add(path
						+ imgName + ".pgm");
		PlanarImage img = JAI.create("fileload", parameterBlock);
		BufferedImage imagePGM = img.getAsBufferedImage();

		// Iterate
		int r, g, b;
		Color gray = new Color(188, 188, 188);
		for (int i = 0; i < image.getWidth(); i++) {
			for (int j = 0; j < image.getHeight(); j++) {
				// pixel cinza -> R=188 G=188 B=188
				if (imagePGM.getRGB(i, j) != gray.getRGB()) {

					vals = new double[data.numAttributes()];
					r = (image.getRGB(i, j) >> 16) & 0xff;
					g = (image.getRGB(i, j) >> 8) & 0xff;
					b = (image.getRGB(i, j)) & 0xff;
		
					
							
					// RGB
					vals[0] = r;
					vals[1] = g;
					vals[2] = b;

					// HSV
					float[] hsbvals= new float[3];
					Color.RGBtoHSB(r, g, b, hsbvals);
					vals[3] = hsbvals[0];
					vals[4] = hsbvals[1];
					vals[5] = hsbvals[2];
					hsbvals = null;
					// YCbCr
					
					vals[6] = rgb2ycbcr(r, g, b)[0];
					vals[7] = rgb2ycbcr(r, g, b)[1];
					vals[8] = rgb2ycbcr(r, g, b)[2];

					// XYZ
					vals[9] = rgb2xyz(r, g, b)[0];
					vals[10] = rgb2xyz(r, g, b)[1];
					vals[11] = rgb2xyz(r, g, b)[2];

					// HSL
					vals[12] = rgb2hsl(r, g, b)[0];
					vals[13] = rgb2hsl(r, g, b)[1];
					vals[14] = rgb2hsl(r, g, b)[2];

					// Lab
					vals[15] = rgb2lab(r, g, b)[0];
					vals[16] = rgb2lab(r, g, b)[1];
					vals[17] = rgb2lab(r, g, b)[2];

					// Luv
					vals[18] = rgb2luv(r, g, b)[0];
					vals[19] = rgb2luv(r, g, b)[1];
					vals[20] = rgb2luv(r, g, b)[2];

					// Positions
					vals[21] = i;
					vals[22] = j;
					
					if (imagePGM.getRGB(i, j) == Color.WHITE.getRGB()) {
						vals[23] = attVals.indexOf("P");
					} else {
						vals[23] = attVals.indexOf("N");
					}
					// add
					data.add(new Instance(1.0, vals));
				} else {
					continue;
				}
			
			}//for 2 img
		}//for 1 img
		img.dispose();
		img = null;
		imagePGM.flush();
		imagePGM =null;
		data.add(new Instance(1.0, vals));
		
		
		}
		// add
	
		// 4. output data
		// System.out.println(data);

		// 5.Saving Data
		// Salvando num arquivo
		saveArff(data,path
						+ arrfFileName + ".arff");
		
		return data;

	}

	private static void saveArff(Instances data, String fileName)
			throws IOException {
		System.out.println("saving Arff file");
		ArffSaver saver = new ArffSaver();
		saver.setInstances(data);
		saver.setFile(new File(fileName));
		saver.writeBatch();

		System.out.println("Data saved on: " + fileName);
	}

	public void openIMG(String src) {

		if (src != null) {
			foto f = new foto(src);
			f.setSize(600, 600);
			f.setVisible(true);
		}

	}

	public static BufferedImage gaussFilter(BufferedImage image, int window) {

		
		BufferedImage dstImage = null;
		GaussianFilter gaussian = new GaussianFilter();
		gaussian.setRadius(window);

		/*
		 * //DisplayImage disp = new DisplayImage(image); //disp.run(image);
		 * 
		 * BufferedImage bfimg = gaussian.filter(image, dstImage); isplayImage
		 * disp2 = new DisplayImage(bfimg); disp2.run(bfimg); return bfimg;
		 */
		return gaussian.filter(image, dstImage);
	}
	
	private static PlanarImage convertColorSpace(PlanarImage
			planarImageInput,
			ColorSpace colorSpaceInput, ColorSpace colorSpaceOutput) {

			ColorModel colorModelInput = RasterFactory.createComponentColorModel(
			planarImageInput.getSampleModel().getDataType(),
			colorSpaceInput, false, false, Transparency.OPAQUE);

			ImageLayout imageLayoutInput = new ImageLayout();
			imageLayoutInput.setColorModel(colorModelInput);
			RenderingHints RenderingHintsInput = new RenderingHints(
			JAI.KEY_IMAGE_LAYOUT, imageLayoutInput);
			ParameterBlock parameterBlockInput = new ParameterBlock();
			parameterBlockInput.addSource(planarImageInput);
			parameterBlockInput.add(planarImageInput.getSampleModel().getDataType());
			PlanarImage planarInputImageInputWithProfile = JAI.create("format",
			parameterBlockInput, RenderingHintsInput);

			ColorModel colorModelOutput = RasterFactory.createComponentColorModel(planarInputImageInputWithProfile.getSampleModel().getDataType(), colorSpaceOutput,
			false, false, Transparency.OPAQUE);

			ImageLayout imageLayoutOutput = new ImageLayout();
			imageLayoutOutput.setSampleModel(colorModelOutput.createCompatibleSampleModel(planarInputImageInputWithProfile.getWidth(), planarInputImageInputWithProfile.getHeight()));
			RenderingHints renderingHintsOutput = new RenderingHints(
			JAI.KEY_IMAGE_LAYOUT, imageLayoutOutput);
			ParameterBlock parameterBlockOutput = new ParameterBlock();
			parameterBlockOutput.addSource(planarInputImageInputWithProfile);
			parameterBlockOutput.add(colorModelOutput);

			return JAI.create("ColorConvert", parameterBlockOutput,
			renderingHintsOutput);
			}


	public static void main(String[] args) throws Exception {	
		

		
		
	
		
		 
		
		
	   int rgb[] = new int[3];
	   float[] HSL,XYZ,LAB,LUV = new float[3];
	   float[] HSV = new float[3];
	   Double[] YcbCr = new Double [3];
	   
	   rgb[0]= 100;
	   rgb[1]= 100;
	   rgb[2]= 100;
	   
	   //HSV = rgb2hsv(rgb[0], rgb[1], rgb[2]);
	   Color.RGBtoHSB(rgb[0], rgb[1],rgb[2], HSV);
	   YcbCr = rgb2ycbcr(rgb[0], rgb[1], rgb[2]);
	   HSL = rgb2hsl(rgb[0], rgb[1], rgb[2]);
	   XYZ = rgb2CIEXYZ(rgb[0], rgb[1], rgb[2]);
	   LAB = rgb2CIELab(rgb[0], rgb[1], rgb[2]);
	   LUV = rgb2luv(rgb[0], rgb[1], rgb[2]);
	   
	   
	   
	   
	   System.out.println( "RGB:   R="+rgb[0]+" G= "+ rgb[1] + " B= "+ rgb[2] );
	   System.out.println("---------------------------------------------------------------------");
	   System.out.println( "HSV:   H="+HSV[0]+" S= "+ HSV[1] + " V= "+ HSV[2] );
	   System.out.println("---------------------------------------------------------------------");
	   System.out.println( "YCbCr: Y="+YcbCr[0]+" Cb= "+ YcbCr[1] + " Cr= "+ YcbCr[2] );
	   System.out.println("---------------------------------------------------------------------");
	   System.out.println( "HSL:   H="+HSL[0]+" S= "+ HSL[1] + " L= "+ HSL[2] );
	   System.out.println("---------------------------------------------------------------------");
	   System.out.println( "XYZ:   X="+XYZ[0]+" Y= "+ XYZ[1] + " Z= "+ XYZ[2] );
	   System.out.println("---------------------------------------------------------------------");
	   System.out.println( "Lab:   L="+LAB[0]+" a= "+ LAB[1] + " b= "+ LAB[2] );
	   System.out.println("---------------------------------------------------------------------");
	   System.out.println( "Luv:   L="+LUV[0]+" u= "+ LUV[1] + " v= "+ LUV[2] );
	   
//		long initMilisecond = System.currentTimeMillis();
//			System.out.println(initMilisecond);
//
//		String path,path2 = new String();
//		
//		// caminho para grama
//		path = "/Volumes/Jovan/Banco de Imagens Monografia/experiments/grass/imagesets/train/";
//	//	path2 = "/Volumes/Jovan/Banco de Imagens Monografia/experiments/grass/imagesets/test/";
//		ArrayList<String> imgNames = new ArrayList<String>();
////		ArrayList<String> imgNames2 = new ArrayList<String>();
////		//Add Test Group
////		
////		imgNames2.add("BraxCro080");
////		imgNames2.add("BraxCro152");
////		imgNames2.add("BraxCro300");
////		imgNames2.add("BraxCro430");
////		imgNames2.add("BraxCro562");
////		imgNames2.add("BraxJap040");
////		imgNames2.add("BraxJap191");
////		imgNames2.add("BraxJap295");
////		imgNames2.add("BraxJap551");
////		imgNames2.add("BraxJap712");
////		imgNames2.add("Henry05");
////		imgNames2.add("Henry17");
////		imgNames2.add("Henry22");
////		imgNames2.add("Henry25");
////		imgNames2.add("Henry67");
////		imgNames2.add("PortxAng002");
////		imgNames2.add("PortxAng0058");
////		imgNames2.add("PortxAng0332");
////		imgNames2.add("PortxAng0419");
////		imgNames2.add("PortxAng1108");
////		imgNames2.add("Zlatan067");
////		imgNames2.add("Zlatan100");
////		imgNames2.add("Zlatan142");
////		imgNames2.add("Zlatan377");
////		imgNames2.add("Zlatan490");
//		
//		
//		
//		
//		
//		//Add Train Group
//		
//		imgNames.add("BraxCro001");
//		imgNames.add("BraxCro140");
//		imgNames.add("BraxCro222");
//		imgNames.add("BraxCro390");
//		imgNames.add("BraxCro470");
//		imgNames.add("BraxJap001");
//		imgNames.add("BraxJap080");
//		imgNames.add("BraxJap212");
//		imgNames.add("BraxJap340");
//		imgNames.add("BraxJap624");
//		//Obs: Henry a imagem pgm tem tamanho 360x230 e a img jpg 360 x 240
//		//imgNames.add("Henry02");
//		imgNames.add("Henry09");
//		imgNames.add("Henry18");
//		imgNames.add("Henry24");
//		imgNames.add("Henry66");
//		imgNames.add("PortxAng001");
//		imgNames.add("PortxAng003");
//		imgNames.add("PortxAng0220");
//		imgNames.add("PortxAng0364");
//		imgNames.add("PortxAng0484");
//		imgNames.add("Zlatan037");
//		imgNames.add("Zlatan077");
//		imgNames.add("Zlatan113");
//		imgNames.add("Zlatan190");
//		imgNames.add("Zlatan467");
//		
//		
//		
//	
//		
//		
//		
//		// caminho para céu
//		/*
//		 path ="/Volumes/Jovan/Banco de Imagens Monografia/experiments/skin/imagesets/test/";
//		 ArrayList<String> imgNames = new ArrayList<String>();
//		 imgNames.add("3921");
//		 */
//
//			Instances data = createArffFile(path, imgNames,"train");
//			//Instances data2 = createArffFile(path2, imgNames2,"test");
//			
//
//			PrincipalComponents pca = new PrincipalComponents();
//			
//			
//			pca.setVarianceCovered(0.95);
//			pca.setMaximumAttributeNames(5);
//			
//
//			Ranker ranker = new Ranker();
//			ranker.setNumToSelect(-1);
//
//	
//
//			weka.filters.supervised.attribute.AttributeSelection filter = new weka.filters.supervised.attribute.AttributeSelection();
//			filter.setEvaluator(pca);
//			filter.setSearch(ranker);
//			filter.setInputFormat(data);
//
//			System.out.println("Runing Principal Component Analyse");
//			System.out.println("getThreshold --> " + ranker.getThreshold());
//			System.out.println("-N " + ranker.getNumToSelect());
//
//
//			Instances newData = Filter.useFilter(data, filter);
//			pca.getCapabilities();
//			
//
//			saveArff(newData, path + "train" +"_PCA.arff");
//		 // saveArff(newData, path + "test" +"_PCA.arff");
//		  
//			System.out.println("Duração: "+(System.currentTimeMillis() - initMilisecond));
			

	}

}
