//
//  ViewController.swift
//  ParticleMouse
//
//  Created by Colin Edwards on 6/8/18.
//  Copyright © 2018 Recursive Penguin. All rights reserved.
//

import Cocoa
import SwiftSocket

class ViewController: NSViewController {

    lazy var window: NSWindow = self.view.window!
    var mouseLocation: NSPoint {
        return NSEvent.mouseLocation
    }
    
    var oldMouseLocationX: CGFloat = 0
    var oldMouseLocationY: CGFloat = 0

    
    var click: UInt8 = 0
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        self.oldMouseLocationX = self.mouseLocation.x
        self.oldMouseLocationY = self.mouseLocation.y

        let client = UDPClient(address: "10.42.42.177", port: 8888)
        client.enableBroadcast()
        
        Timer.scheduledTimer(withTimeInterval: 0.08, repeats: true) { (Timer) in
            
            let diffX = Int((self.oldMouseLocationX - self.mouseLocation.x) / 12.0)
            let diffY = Int((self.oldMouseLocationY - self.mouseLocation.y) / 12.0)
            
            self.oldMouseLocationX = self.mouseLocation.x
            self.oldMouseLocationY = self.mouseLocation.y
            
            print("diff:", String(format: "%d, %d", diffX, diffY))
            
            if diffX > 254 || diffY > 244 {
                return
            }
            
            var snesMouseX:UInt8 = UInt8(abs(diffX))
            var snesMouseY:UInt8 = UInt8(abs(diffY))
            
            snesMouseX = snesMouseX.setb7(diffX > -1 ? 1 : 0)
            
            snesMouseY = snesMouseY.setb7(diffY < 0 ? 1 : 0)
            
            let data: Data = Data.init(bytes: [self.click, snesMouseX, snesMouseY])
            let result = client.send(data: data)
            //print(result)
            //print("mouseLocation:", String(format: "%d, %d", snesMouseX, snesMouseY))
        }
    
    }

    override func mouseDown(with event: NSEvent) {
        click = 1
    }
    
    override func mouseUp(with event: NSEvent) {
        click = 0
    }
}

