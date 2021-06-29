 class SDK {
   static {
      // 加载libagentproxy
      System.load("/Users/momo/Office/mesh-inside-bar/sdk-java-agent.so");
   }

   private native void agentIOFromJava();

   public static void main(String[] args) {
      // 调用libagentproxy.so
      new SDK().agentIOFromJava();  // invoke the native method
   }
}