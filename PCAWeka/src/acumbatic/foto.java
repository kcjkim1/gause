package acumbatic;

import java.awt.BorderLayout;
import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileOutputStream;
import java.net.URL;

import javax.swing.ImageIcon;
import javax.swing.JApplet;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;


public class foto extends JFrame{
	
	private ImageIcon image;
	private JPanel scaleJPanel;
	private JLabel percentJLabel;
	private JTextField scaleInputJTextField;
	private JButton scaleChangeJButton;
	private double scaleValue = 1.0;
	
	public foto(URL caminho){
		scaleJPanel = new JPanel();
		percentJLabel = new JLabel("Porcentagem da escala: ");
		scaleInputJTextField = new JTextField("100");
		scaleChangeJButton = new JButton("Mudar Escala");
		
		scaleJPanel.add(percentJLabel);
		scaleJPanel.add(scaleInputJTextField);
		scaleJPanel.add(scaleChangeJButton);
		add( scaleJPanel, BorderLayout.NORTH);
		
		
		scaleChangeJButton.addActionListener(
		new ActionListener() {
			
			@Override
				public void actionPerformed(ActionEvent arg0) {
					scaleValue = Double.parseDouble(scaleInputJTextField.getText())/100.0;
					repaint();
				}
			}		
		);
		
		
		//image = new ImageIcon("C:\\Lumentech\\forrocaju2010.jpg");
		image = new ImageIcon(caminho);
		add(new DrawJPanel(), BorderLayout.CENTER);
	}
	
	public foto(String caminho){
		scaleJPanel = new JPanel();
		percentJLabel = new JLabel("Porcentagem da escala: ");
		scaleInputJTextField = new JTextField("100");
		scaleChangeJButton = new JButton("Mudar Escala");
		
		scaleJPanel.add(percentJLabel);
		scaleJPanel.add(scaleInputJTextField);
		scaleJPanel.add(scaleChangeJButton);
		add( scaleJPanel, BorderLayout.NORTH);
		
		
		scaleChangeJButton.addActionListener(
		new ActionListener() {
			
			@Override
				public void actionPerformed(ActionEvent arg0) {
					scaleValue = Double.parseDouble(scaleInputJTextField.getText())/100.0;
					repaint();
				}
			}		
		);
		
		
		
		image = new ImageIcon(caminho);
		add(new DrawJPanel(), BorderLayout.CENTER);
	}
	
	
	
	private class DrawJPanel extends JPanel{
		
		public void paintComponent(Graphics g){
			super.paintComponent(g);
			
			double spareWidth = getWidth() - scaleValue * image.getIconWidth();
			double spareHeight = getHeight() - scaleValue * image.getIconHeight();
			
			g.drawImage(image.getImage(), (int) spareWidth/2, (int) spareHeight/2, (int) (image.getIconWidth() * scaleValue),(int) (image.getIconHeight() * scaleValue), this);
			
		}
		
		
	}

}
